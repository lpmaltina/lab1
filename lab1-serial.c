/* Последовательная программа.
Читает входные данные из директории input.
Сохраняет координаты точек в течение указанного количества временных шагов
в директорию output */
#include "file-utils.h"
#include "timer.h"
#include "vec.h"

#define DT 0.05
#define EPS 0.00000001
#define G 6.6743e-11

/* Функция для вычисления ускорений.
Обновляет глобальный массив accelerations. */
void computeAccelerations(){
    int i, j;
    double denominator;
    
    for (i = 0; i < bodies; ++i){
        accelerations[i].x = 0;
        accelerations[i].y = 0;
        for (j = 0; j < bodies; ++j){
            if (i != j){
                denominator = pow(
                    modVector(subtractVectors(positions[i], positions[j])), 3
                );
                if (denominator < EPS){
                    denominator = EPS;
                }
                accelerations[i] = addVectors(
                    accelerations[i],
                    scaleVector(
                        G * masses[j] / denominator,
                        subtractVectors(positions[j], positions[i])
                    )
                );
            }
        }
    }
}

/* Функция для вычисления скоростей.
Обновляет глобальный массив velocities. */
void computeVelocities(){
    int i;
    for (i = 0; i < bodies; ++i){
        velocities[i] = addVectors(
            velocities[i],
            scaleVector(DT, accelerations[i])
        );
    }
}

/* Функция для вычисления координат точек.
Обновляет глобальный массив positions. */
void computePositions(){
    int i;
    for (i = 0; i < bodies; ++i){
        positions[i] = addVectors(
            positions[i],
            scaleVector(DT, velocities[i])
        );
    }
}

/* Функция, которая выполняет вычисления для одного временного шага. */
void simulate(){
    computeAccelerations();
    computePositions();
    computeVelocities();
}

/* Функция, которая сохраняет координаты точек
в течение указанного количества временных шагов в директорию output.

Формат имени файла с результатами:
output-serial-<количество точек>-<количество временных шагов>.csv.

Рассматриваемое количество точек: 64, 128, 256, 512, 1024.
Рассматриваемое количество временных шагов: 10, 100, 1000. */
int main(){
    int timeStep;
    int timeSteps;
    char inputFileName[30] = {0};
    char outputFileName[30] = {0};
    FILE *outputFile;
    double time;
    double start, end;

    for (bodies = 64; bodies <= 1024; bodies *= 2){
        sprintf(inputFileName, "input/input-%d.txt", bodies);
        masses = (double *)malloc(bodies * sizeof(double));
        positions = (vec *)malloc(bodies * sizeof(vec));
        velocities = (vec *)malloc(bodies * sizeof(vec));
        accelerations = (vec *)malloc(bodies * sizeof(vec));
        for (timeSteps = 10; timeSteps <= 1000; timeSteps *= 10){
            initiateSystem(inputFileName);
            // sprintf(
            //     outputFileName,
            //     "output/output-serial-%d-%d.csv",
            //     bodies,
            //     timeSteps
            // );
            // outputFile = fopen(outputFileName, "w+");
            // writeHeader(outputFile);
            
            time = 0;
            GET_TIME(start);
            for (timeStep = 1; timeStep < timeSteps + 1; ++timeStep){
                simulate();
                // writeTimeStepInfo(outputFile, timeStep);
            }
            GET_TIME(end);
            time += end - start;

            // fclose(outputFile);
            printf("bodies=%d, timeSteps=%d: %f\n", bodies, timeSteps, time);
            
        }
        free(masses);
        free(accelerations);
        free(velocities);
        free(positions);
    }
    return 0;
}
