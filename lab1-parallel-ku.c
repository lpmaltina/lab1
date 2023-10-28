#include <pthread.h>

#include "universal.h"


#define FILE_OUTPUT "output/output-parallel-ku-10-10"


int thread_count = 800;
double edgesPerThread = 0;

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
	int bgg;
	int end;

	/* int bgg = nth * (bodies / thread_count); */
	/* int end = 0; */
	/* if (thread_count - 1 != nth) { */
		/* end = bgg + (bodies / thread_count); */
	/* } else { */
		/* end = bodies; */
	/* } */

/* How many bodies per thread do we need? In the best case, every       */
/* thread shall process approximately equal number of edges between.    */
/* vertices. Number of edges between vertices is n * (n - 1) / 2.       */
/* */
/* bodies * (bodies - 1) / 2 = edges                                    */
/* (1) edgesPerThread = edges / thread_count  =>                        */
/*  =>  edgesThreadFst = edges / thread_count, however                  */
/* */
/* edgesThreadFst = vertsThreadFst * (vertsThreadFst - 1) / 2     =>    */
/*  =>  vertsThreadFst^2 - vertsThreadFst - 2*edgesThreadFst = 0  =>    */
/*  =>  vertsThreadFst = 1 +/- sqrt(1 + 4*2*edgesThreadFst), however    */
/* */
/* 1 - sqrt(1 + 4*2*edgesThreadFst) is always negative, so              */
/* vertsThreadFst = 1 + sqrt(1 + 4*2*edgesThreadFst)                    */
/* */
/* using (1) we get:                                                    */
/* vertsThreadFst = 1 + sqrt(1 + 8*(1 * (edges / thread_count)), SO     */
/* 1ST THREAD USES VERTS FROM 0 to vertsThreadFst - 1                   */
/* */
/* vertsThreadSnd = 1 + sqrt(1 + 8*(2 * (edges / thread_count)), SO     */
/* 2nd thread should(??) use verts from 0 to vertsThreadSnd - 1, BUT    */
/* from 0 to vertsThreadFst - 1 are already taken by 1st thread, THEN   */
/* 2ND THREAD USES VERTS FROM vertsThreadFst to vertsThreadSnd - 1      */
/* */
/* vertsThreadTrd = 1 + sqrt(1 + 8*(3 * (edges / thread_count)), SO     */
/* 3rd thread should(??) use verts from 0 to vertsThreadTrd - 1, BUT    */
/* from 0 to vertsThreadFst - 1 are already taken by 1st thread, AND    */
/* from vertsThreadFst to vertsThreadSnd -1 taken by 2nd thread, THEN   */
/* 3RD THREAD USES VERTS FROM vertsThreadSnd to vertsThreadTrd - 1      */
/* */
/* vertsThreadFth = 1 + sqrt(1 + 8*(4 * (edges / thread_count)), SO     */
/* 4th thread should(??) use verts from 0 to vertsThreadFth - 1, BUT    */
/* from 0 to vertsThreadFst - 1 are already taken by 1st thread, AND    */
/* from vertsThreadFst to vertsThreadSnd -1 taken by 2st thread, AND    */
/* from vertsThreadSnd to vertsThreadTrd -1 taken by 3st thread, THEN   */
/* 4TH THREAD USES VERTS FROM vertsThreadTrd to vertsThreadTrd - 1      */
/* */
/* and so on... */

	if (nth == 0) {
		bgg = 0;
	} else {
		bgg = (1 + sqrt(1 + 4*(nth)*edgesPerThread)) / 2.;
	}

	if (nth == bodies - 1) {
		end = bodies;
	} else if (nth > bodies - 1) {
		bgg = bodies;
		end = bodies;
	} else {
		end = (1 + sqrt(1 + 4*(nth + 1)*edgesPerThread)) / 2.;
	}

	printf("nth %3.1lli, bgg %3.1i, end %3.1i, verts %3.1i\n",
		   nth,
		   2*((bodies - 1) / 2) - end + (bodies % 2 != 1) + 1,
		   2*((bodies - 1) / 2) - bgg + (bodies % 2 != 1) + 1,
		   (end * (end - 1) / 2) - (bgg * (bgg - 1) / 2));

	bgg = 2*((bodies - 1) / 2) - bgg + (bodies % 2 != 1) + 1;
	end = 2*((bodies - 1) / 2) - end + (bodies % 2 != 1) + 1;

	bgg = bgg + end;
	end = bgg - end;
	bgg = bgg - end;

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
	edgesPerThread = bodies * (bodies - 1) / (double) thread_count;

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
