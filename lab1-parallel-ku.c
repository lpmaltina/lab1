#include <pthread.h>

#include "universal.h"


#define FILE_OUTPUT "output/output-parallel-ku-10-10"


int thread_count = 7;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier = {0};


void computeAccels(int bgg, int end, int nth)
{
    int i;
	int j;

	long double denom = 0.l;
	vec vec_pose_diff = {0};
	vec vec_without_mass = {0};
	vec vec_acc_i = {0};
	vec vec_acc_j = {0};
	vec vec_acc_ij = {0};
	vec vec_acc_ji = {0};

    for (i = bgg; i < end; i++) {
		vec_acc_i = vecEmpty;

        for (j = i + 1; j < bodies; j++) {
			vec_pose_diff = subVecs(poses[j], poses[i]);
			denom = pow(modVec(vec_pose_diff), 3);
			if (denom < EPS) {
				denom = EPS;
			}
			vec_without_mass = scaleVec(GravConst / denom, vec_pose_diff);

			vec_acc_ij = scaleVec(masses[j], vec_without_mass);
			vec_acc_ji = scaleVec(-masses[i], vec_without_mass);
			vec_acc_i = addVecs(vec_acc_i, vec_acc_ij);

			pthread_mutex_lock(&mutex);
			accels[j] = addVecs(accels[j], vec_acc_ji);
			putchar(nth + 48);
			pthread_mutex_unlock(&mutex);
        }
		pthread_mutex_lock(&mutex);
		accels[i] = addVecs(accels[i], vec_acc_i);
		pthread_mutex_unlock(&mutex);
    }

	return;
}

void computePoses(int bgg, int end)
{
	int i;
	for (i = bgg; i < end; i++) {
		pthread_mutex_lock(&mutex);
		poses[i] = addVecs(poses[i], scaleVec(DT, vels[i]));
		pthread_mutex_unlock(&mutex);
	}
	return;
}

void computeVels(int bgg, int end)
{
	int i;
	for (i = bgg; i < end; i++) {
		pthread_mutex_lock(&mutex);
		vels[i] = addVecs(vels[i], scaleVec(DT, accels[i]));
		pthread_mutex_unlock(&mutex);
	}
	return;
}

void* routine(void* arg)
{
	int i;
	int j;

	long long nth = (long long) arg;
	int bgg = nth * (bodies / thread_count);
	int end = 0;
	if (thread_count - 1 != nth) {
		end = bgg + (bodies / thread_count);
	} else {
		end = bodies;
	}

	for (i = 0; i < timeSteps; i++) {

		pthread_barrier_wait(&barrier);

		for (j = bgg; j < end; j++) {
			accels[j] = vecEmpty;
		}

		pthread_barrier_wait(&barrier);
		computeAccels(bgg, end, nth);

		pthread_barrier_wait(&barrier);
		computePoses(bgg, end);

		pthread_barrier_wait(&barrier);
		computeVels(bgg, end);

		pthread_barrier_wait(&barrier);
		if (nth != 0) {
			continue;
		}

		fprintf(fOut, "%d\n", i + 1);
		for (j = 0; j < bodies; j++) {
			fprintf(fOut, "%.15Lf, %.15Lf\n", poses[j].x, poses[j].y);
		}
		fprintf(fOut, "\n");
		printf("\n");
	}

	pthread_exit(NULL);
}

int main()
{
	rslt = initiateSystem(FILE_INPUT, FILE_OUTPUT);
	if (rslt != 0) {
		return 1;
	}

	pthread_barrier_init(&barrier, NULL, thread_count);

	pthread_t* pthread_handlers = calloc(thread_count, sizeof(pthread_t));
	for (long long i = 0; i < thread_count; i++) {
		pthread_create(&pthread_handlers[i], NULL, routine, (void*) i);
	}
	for (long long i = 0; i < thread_count; i++) {
		pthread_join(pthread_handlers[i], NULL);
	}
	free(pthread_handlers);

	terminateSystem();

    return 0;
}
