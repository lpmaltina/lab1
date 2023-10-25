#include <pthread.h>

#include "universal.h"


int thread_count = 3;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier = {0};


void computeAccels(int bgg, int end, int nth)
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

    for (i = bgg; i < end; i++) {

		/* pthread_mutex_lock(&mutex); */
		vec_acc_i = accels[i];
		/* pthread_mutex_unlock(&mutex); */

		/* vec_acc_i = vecEmpty; */
        for (j = i + 1; j < bodies; j++) {

			if (i == j) { continue; }

			vec_pose_diff = subVecs(poses[j], poses[i]);
			denom = pow(modVec(vec_pose_diff), 3);
			/* if (denom < eps){ */
				/* denom = eps; */
			/* } */
			vec_without_mass = scaleVec(GravConst / denom, vec_pose_diff);

			/* pthread_mutex_lock(&mutex); */
			vec_acc_j = accels[j];
			/* pthread_mutex_unlock(&mutex); */


			vec_acc_ij = scaleVec(masses[j], vec_without_mass);
			vec_acc_ji = scaleVec(-masses[i], vec_without_mass);
			vec_acc_i = addVecs(vec_acc_i, vec_acc_ij);
			vec_acc_j = addVecs(vec_acc_j, vec_acc_ji);

			/* pthread_mutex_lock(&mutex); */
			accels[j] = vec_acc_j;
			/* pthread_mutex_unlock(&mutex); */

			/* accels[i] = vec_acc_i; */
			putchar(nth + 48);
        }
		/* pthread_mutex_lock(&mutex); */
		accels[i] = vec_acc_i;
		/* pthread_mutex_unlock(&mutex); */
    }
}

void* routine(void* arg)
{
	int i = 0;
	int j = 0;

	long long nth = (long long) arg;
	int bgg = nth * (bodies / thread_count);
	int end = bgg + (bodies / thread_count);


	for (i = 0; i < 1; i++) {
		pthread_mutex_lock(&mutex);
		computeAccels(bgg, end, nth);
		pthread_mutex_unlock(&mutex);

		pthread_barrier_wait(&barrier);
	}

	pthread_exit(NULL);
}

int main()
{
	rslt = initiateSystem(
		"input/input-10-10",
		"output/output-parallel-ku-10-10");
	if (rslt != 0) { return 1; }

	pthread_barrier_init(&barrier, NULL, thread_count);

	pthread_t* pthread_handlers = calloc(thread_count, sizeof(pthread_t));
	for (long long i = 0; i < thread_count; i++) {
		pthread_create(&pthread_handlers[i], NULL, routine, (void*) i);
	}
	for (long long i = 0; i < thread_count; i++) {
		pthread_join(pthread_handlers[i], NULL);
	}
	free(pthread_handlers);

	printf("\n");
	fprintf(fOut, "1\t");
	for (int i = 0; i < bodies; i++) {
		fprintf(fOut, "%.20lf\t%.20lf\t", accels[i].x, accels[i].y);
	}
	fprintf(fOut, "\n");

	terminateSystem();

    return 0;
}
