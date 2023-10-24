#include <pthread.h>

#include "universal.h"


int thread_count = 2;

pthread_mutex_t mutex = {0};
pthread_barrier_t barrier = {0};
pthread_rwlock_t rwlock = {0}; /* used in computeAccels */

vec *nxt_poses = NULL;
vec *nxt_vels = NULL;


void computeAccels(int start, int finish)
{
    int i = 0;
	int j = 0;
	double denom = 0.;

	vec vec_pose_diff = {0};
	vec vec_without_mass = {0};
	vec vec_acc_i = {0};
	vec vec_acc_j = {0};
	vec vec_acc_ij = {0};
	vec vec_acc_ji = {0};

    for (i = start; i < finish; i++) {


		vec_acc_i = accels[i];


        for (j = i + 1; j < bodies; j++) {

			vec_pose_diff = subVecs(poses[j], poses[i]);

			denom = pow(modVec(vec_pose_diff), 3);
			if (denom < eps){
				denom = eps;
			}

			vec_without_mass = scaleVec(GravConst / denom, vec_pose_diff);


			vec_acc_j = accels[j];


			vec_acc_ij = scaleVec(masses[j], vec_without_mass);
			vec_acc_ji = scaleVec(-masses[i], vec_without_mass);
			vec_acc_i = addVecs(vec_acc_i, vec_acc_ij);
			vec_acc_j = addVecs(vec_acc_j, vec_acc_ji);


			accels[j] = vec_acc_j;

        }

        accels[i] = vec_acc_i;

    }
}

void computeVels(int start, int finish)
{
    int i = 0;
    for (i = start; i < finish; i++) {
        nxt_vels[i] = addVecs(vels[i], scaleVec(DT, accels[i]));
	}
}

void computePoses(int start, int finish)
{
    int i = 0;
    for (i = start; i < finish; i++) {
        nxt_poses[i] = addVecs(poses[i], scaleVec(DT, vels[i]));
	}
}

void updateArrays(int start, int finish)
{
    for (int i = start; i < finish; ++i){
        poses[i] = nxt_poses[i];
        vels[i] = nxt_vels[i];
    }
}

void* routine(void* nthread)
{
	int i = 0;
	int j = 0;

    long long my_nthread = (long long) nthread;
    int my_points = bodies / thread_count;
    int my_first_point = my_nthread * my_points;
    int my_last_point = my_first_point + my_points;

	printf("my  first  point's  x:  %f\n", poses[my_first_point].x);

     for (i = 0; i < timeSteps; ++i) {

		pthread_mutex_lock(&mutex);

        for (j = my_first_point; j < my_last_point; ++j) {
            accels[j] = vecEmpty;
        }

        computeAccels(my_first_point, my_last_point);
        computePoses(my_first_point, my_last_point); 
        computeVels(my_first_point, my_last_point);

		pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier);
        updateArrays(my_first_point, my_last_point);
		if (my_nthread == 0) {
			printf("my first point's x now: %i %f\n", i, poses[my_first_point].x);
		}
        pthread_barrier_wait(&barrier);

        if (my_nthread == 0){
            fprintf(fOut, "%d\t", i + 1);
            for (j = 0; j < bodies; j++){
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
    rslt = initiateSystem(
		"input/input-10-10",
		"output/output-parallel-10-10");
	if (rslt != 0) { return 1; }

    nxt_poses = calloc(bodies, sizeof(struct vec));
    nxt_vels = calloc(bodies, sizeof(struct vec));
    
    pthread_barrier_init(&barrier, NULL, thread_count);

	pthread_mutex_init(&mutex, NULL);

	pthread_rwlock_init(&rwlock, NULL);
    pthread_t* pthread_handlers = calloc(thread_count, sizeof(pthread_t));
    for (long long i = 0; i < thread_count; ++i){
        pthread_create(&pthread_handlers[i], NULL, routine, (void*) i);
    }

    for (long long i = 0; i < thread_count; ++i){
        pthread_join(pthread_handlers[i], NULL);
    }
    free(pthread_handlers);
	pthread_rwlock_destroy(&rwlock);

    pthread_barrier_destroy(&barrier);
    fclose(fOut);
    free(masses);
    free(accels);
    free(vels);
    free(poses);
    free(nxt_poses);
    free(nxt_vels);
    return 0;
}
