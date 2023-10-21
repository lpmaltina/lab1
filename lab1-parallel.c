#include <pthread.h>

#include "universal.h"


int thread_count = 5;
pthread_barrier_t barrier = {0};
pthread_mutex_t mutex, barrier_mutex = {0};
pthread_cond_t cond_var = {0};
pthread_rwlock_t rwlock = {0}; /* used in computeAccelerations */


vec *next_poses = NULL, *next_velocities = NULL;
int counter = 0;


void computeAccelerations(int start, int finish)
{
    int i, j;

	vec accelerationI = {0};
	vec accelerationJ = {0};

    for (i = start; i < finish; i++) {

		pthread_rwlock_rdlock(&rwlock);
		accelerationI = accelerations[i];
		pthread_rwlock_unlock(&rwlock);

        for (j = i + 1; j < bodies; j++) {

			vec posesDiff = subVecs(poses[j], poses[i]);

			double denominator = pow(modVec(posesDiff), 3);
			if (denominator < eps){
				denominator = eps;
			}

			vec withoutMass =
				scaleVec(GravConstant / denominator, posesDiff);

			pthread_rwlock_rdlock(&rwlock);
			accelerationJ = accelerations[j];
			pthread_rwlock_unlock(&rwlock);

			vec accelerationIJ = scaleVec(masses[j], withoutMass);
			vec accelerationJI = scaleVec(-masses[i], withoutMass);
			accelerationI = addVecs(accelerationI, accelerationIJ);
			accelerationJ = addVecs(accelerationJ, accelerationJI);

			pthread_rwlock_wrlock(&rwlock);
			accelerations[j] = accelerationJ;
			pthread_rwlock_unlock(&rwlock);
        }
		pthread_rwlock_wrlock(&rwlock);
        accelerations[i] = accelerationI;
		pthread_rwlock_unlock(&rwlock);
    }
}


void computeVelocities(int start, int finish)
{
    int i;
    for (i = start; i < finish; i++)
        next_velocities[i] = addVecs(velocities[i], scaleVec(DT, accelerations[i]));
}


void computePoses(int start, int finish)
{
    int i;
    for (i = start; i < finish; i++) {
        next_poses[i] = addVecs(poses[i], scaleVec(DT, velocities[i]));
	}
}

void updateArrays(int start, int finish){
    for (int i = start; i < finish; ++i){
        poses[i] = next_poses[i];
        velocities[i] = next_velocities[i];
    }
}

void* routine(void* nthread){
    long long my_nthread = (long long) nthread;
    int my_points = bodies / thread_count;
    int my_first_point = my_nthread * my_points;
    int my_last_point = my_first_point + my_points;
    
     for (int i = 0; i < timeSteps; ++i){

		pthread_mutex_lock(&mutex);

        for (int j = my_first_point; j < my_last_point; ++j) {
            accelerations[j] = vecEmpty;
        }

        computeAccelerations(my_first_point, my_last_point);
        computePoses(my_first_point, my_last_point); 
        computeVelocities(my_first_point, my_last_point);

		pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier);
        updateArrays(my_first_point, my_last_point);
        pthread_barrier_wait(&barrier);
        if (my_nthread == 0){
            fprintf(fOut, "%d\t", i + 1);
            for (int j = 0; j < bodies; j++){
                fprintf(fOut, "%.20f\t%.20f\t", poses[j].x, poses[j].y);
            }
            fprintf(fOut, "\n");
        }
        pthread_barrier_wait(&barrier);
    }
    return NULL;
}


int main()
{
    initiateSystem(
		"input/input-10-10",
		"output/output-parallel-10-10");

    fprintf(fOut, "t\t");
    for (int j = 1; j < bodies + 1; ++j){
        fprintf(fOut, "x%d\ty%d\t", j, j);
    }
    fprintf(fOut, "\n");

    next_poses = (vec *)malloc(bodies * sizeof(vec));
    next_velocities = (vec *)malloc(bodies * sizeof(vec));
    
    pthread_barrier_init(&barrier, NULL, thread_count);

	pthread_rwlock_init(&rwlock, NULL);
    pthread_t* pthread_handles = malloc(thread_count * sizeof(pthread_t));
    for (long long i = 0; i < thread_count; ++i){
        pthread_create(&pthread_handles[i], NULL, routine, (void*) i);
    }

    for (long long i = 0; i < thread_count; ++i){
        pthread_join(pthread_handles[i], NULL);
    }
    free(pthread_handles);
	pthread_rwlock_destroy(&rwlock);

    pthread_barrier_destroy(&barrier);
    fclose(fOut);
    free(masses);
    free(accelerations);
    free(velocities);
    free(poses);
    free(next_poses);
    free(next_velocities);
    return 0;
}
