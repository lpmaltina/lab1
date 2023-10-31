#include <pthread.h>
#include "file-utils.h"
#include "timer.h"
#include "vec.h"

#define DT 0.05
#define EPS 0.00000001
#define G 6.6743e-11

pthread_barrier_t barrier;
int threadCount;
FILE* outputFile;
double computationsTime;
int timeSteps;
vec *nextPositions, *nextVelocities;

pthread_mutex_t* mutexes = {0};

/* Функция для вычисления ускорений.
Обновляет часть глобального массив accelerations
с индекса bgg (включительно) до индекса end (невключительно). */
void computeAccelerations(int bgg, int end)
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
		vec_acc_i.x = 0;
		vec_acc_i.y = 0;

        for (j = i + 1; j < bodies; j++) {
			vec_pose_diff = subtractVectors(positions[j], positions[i]);
			denom = pow(modVector(vec_pose_diff), 3);
			if (denom < EPS) {
				denom = EPS;
			}
			vec_without_mass = scaleVector(G / denom, vec_pose_diff);

			vec_acc_ij = scaleVector(masses[j], vec_without_mass);
			vec_acc_ji = scaleVector(-masses[i], vec_without_mass);
			vec_acc_i = addVectors(vec_acc_i, vec_acc_ij);

			pthread_mutex_lock(&mutexes[j]);
			accelerations[j] = addVectors(accelerations[j], vec_acc_ji);
			pthread_mutex_unlock(&mutexes[j]);
        }
		pthread_mutex_lock(&mutexes[i]);
		accelerations[i] = addVectors(accelerations[i], vec_acc_i);
		pthread_mutex_unlock(&mutexes[i]);
    }

	return;
}

/* Функция для вычисления скоростей.
Обновляет часть глобального массив nextVelocities
с индекса start (включительно) до индекса finish (невключительно). */
void computeVelocities(int start, int finish){
    int i;
    for (i = start; i < finish; ++i){
        nextVelocities[i] = addVectors(
            velocities[i],
            scaleVector(DT, accelerations[i])
        );
    }
}


/* Функция для вычисления координат точек.
Обновляет часть глобального массив nextPositions
с индекса start (включительно) до индекса finish (невключительно). */
void computePositions(int start, int finish){
    int i;
    for (i = start; i < finish; ++i){
        nextPositions[i] = addVectors(
            positions[i],
            scaleVector(DT, velocities[i])
        );
    }   
}

/* Функция для записи координат точек и скоростей,
полученных на новом временном шаге.
Обновляет части глобальных массивов positions и velocities
с индекса start (включительно) до индекса finish (невключительно). */
void updateArrays(int start, int finish){
    int i;
    for (i = start; i < finish; ++i){
        positions[i] = nextPositions[i];
        velocities[i] = nextVelocities[i];
    }
}

/* Функция, которая выполняется на каждом потоке.
На каждом временном шаге:

- вычисляет ускорения, обновляя глобальный массив accelerations
с индекса start (включительно) до индекса finish (невключительно);

- вычисляет координаты точек для нового временного шага,
обновляя глобальный массив nextPositions
с индекса start (включительно) до индекса finish (невключительно);

- вычисляет скорости, обновляя глобальный массив nextVelocities
с индекса start (включительно) до индекса finish (невключительно);

- обновляет части глобальных массивов positions и velocities
с индекса start (включительно) до индекса finish (невключительно);

- нулевой поток записывает в output-файл
координаты точек для нового временного шага. 

Время записи в файл вычитается из времени вычислений. */
void* routine(void* nthread)
{
	int i;

    long long nth = (long long) nthread;
	double edgesPerThread = bodies * (bodies - 1) / (double) threadCount;
	int bgg;
	int end;

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

	bgg = 2*((bodies - 1) / 2) - bgg + (bodies % 2 != 1) + 1;
	end = 2*((bodies - 1) / 2) - end + (bodies % 2 != 1) + 1;

	bgg = bgg + end;
	end = bgg - end;
	bgg = bgg - end;

    for (i = 0; i < timeSteps; ++i){
        computeAccelerations(bgg, end);
        computePositions(bgg, end);
        computeVelocities(bgg, end);
        pthread_barrier_wait(&barrier);
        updateArrays(bgg, end);
        pthread_barrier_wait(&barrier);
        // if (nth == 0){
        //     writeTimeStepInfo(outputFile, timeStep + 1);
        // }
    }

    return NULL;
}


/* Функция, которая сохраняет координаты точек
в течение указанного количества временных шагов в директорию output.

Формат имени файла с результатами:
output-parallel-<количество потоков>-<количество точек>-<количество временных шагов>.csv.

Рассматриваемое количество потоков: 1, 2, 4, 8.
Рассматриваемое количество точек: 64, 128, 256, 512, 1024.
Рассматриваемое количество временных шагов: 10, 100, 1000.

Также в файл timings.csv записывается время вычислений в формате:
<кол-во потоков><разделитель><кол-во точек><разделитель><кол-во временных шагов><разделитель><время вычислений (с)>*/
int main(){
    long long i;
    double start, end;
    char inputFileName[30] = {0};
    char outputFileName[40] = {0};
    char timingsFileName[] = "timings.csv";
    FILE* timingsFile;
    timingsFile = fopen(timingsFileName, "w+");
    fprintf(timingsFile, "threads\tbodies\ttimeSteps\ttime\n");

    for (threadCount = 1; threadCount <= 8; threadCount *= 2){
        for (bodies = 64; bodies <= 256; bodies *= 2){
            masses = (double *)malloc(bodies * sizeof(double));
            positions = (vec *)malloc(bodies * sizeof(vec));
            velocities = (vec *)malloc(bodies * sizeof(vec));
            accelerations = (vec *)malloc(bodies * sizeof(vec));
            nextPositions = (vec *)malloc(bodies * sizeof(vec));
            nextVelocities = (vec *)malloc(bodies * sizeof(vec));
			mutexes = malloc(bodies * sizeof(pthread_mutex_t));
			for (i = 0; i < bodies; i++) {
				pthread_mutex_init(&mutexes[i], NULL);
			}

            sprintf(inputFileName, "input/input-%d.txt", bodies);
            for (timeSteps = 10; timeSteps <= 1000; timeSteps *= 10){
                initiateSystem(inputFileName);
                // sprintf(
                //     outputFileName,
                //     "output/output-parallel-%d-%d-%d.csv",
                //     threadCount,
                //     bodies,
                //     timeSteps
                // );
                // outputFile = fopen(outputFileName, "w+");
                // writeHeader(outputFile);

                computationsTime = 0;
                
                GET_TIME(start);
                pthread_barrier_init(&barrier, NULL, threadCount);

                pthread_t* pthreadHandles = (pthread_t *)malloc(
                    threadCount * sizeof(pthread_t)
                );

                for (i = 0; i < threadCount; ++i){
                    pthread_create(&pthreadHandles[i], NULL, routine, (void*) i);
                }

                for (i = 0; i < threadCount; ++i){
                    pthread_join(pthreadHandles[i], NULL);
                }
                free(pthreadHandles);
                
                pthread_barrier_destroy(&barrier);
                GET_TIME(end);
                computationsTime += end - start;

                printf(
                    "threads=%d, bodies=%d, timeSteps=%d\n",
                    threadCount,
                    bodies,
                    timeSteps
                );
                fprintf(
                    timingsFile,
                    "%d%c%d%c%d%c%f\n",
                    threadCount,
                    DELIMITER,
                    bodies,
                    DELIMITER,
                    timeSteps,
                    DELIMITER,
                    computationsTime
                );
                // fclose(outputFile);
            }

			for (i = 0; i < bodies; i++) {
				pthread_mutex_destroy(&mutexes[i]);
			}
			free(mutexes);
            free(masses);
            free(accelerations);
            free(velocities);
            free(positions);  
            free(nextPositions);
            free(nextVelocities);
        }
    }
    fclose(timingsFile);
    return 0;
}
