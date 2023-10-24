/* Код для генерации входных данных.
Записывает сгенерированные данные в директорию input */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Функция, которая возвращает случайное целое число 
от min (включительно) до max (включительно) с шагом step */
int generateInt(int min, int max, int step){
    return min + rand() % ((max - min) / step + 1) * step;
}

/* Функция для генерации входных данных.
Записывает сгенерированные данные в директорию input.

Имена сгенерированных файлов имеют формат:
input-<количество точек>.txt.

Формат файла:
<количество точек>
<масса точки 1>
<координата точки 1 по оси x> <координата точки 1 по оси y>
<проекция скорости точки 1 по оси x> <проекция скорости точки 1 по оси y>
...
<масса точки n>
<координата точки n по оси x> <координата точки n по оси y>
<проекция скорости точки n по оси x> <проекция скорости точки n по оси y>

Координата точки по оси x - случайное число
от -bodies * bodies до bodies * bodies с шагом 1,
где bodies - количество точек.

Координата точки по оси y - случайное число
от -bodies * bodies до bodies * bodies с шагом 1,
где bodies - количество точек.

Масса точек - случайное число из 100000, 200000, ..., 10000000.

Проекция скорости по оси x - случайное число из -100, -99, -98, ..., 100.

Проекция скорости по оси y - случайное число из -100, -99, -98, ..., 100. */
void generateInput(int bodies){
    int minCoord = -bodies * bodies;
    int maxCoord = bodies * bodies;
    int coordStep = 1;
    int minMass = 100000;
    int maxMass = 10000000;
    int massStep = 100000;
    int minSpeed = -100;
    int maxSpeed = 100;
    int speedStep = 1;
    char fileName[30];
    int body;
    sprintf(fileName, "input/input-%d.txt", bodies);

    FILE *fp = fopen(fileName, "w");
    fprintf(fp, "%d\n", bodies);

    for (body = 0; body < bodies; ++body){
        fprintf(
            fp, "%d\n%f %f\n%f %f\n",
            generateInt(minMass, maxMass, massStep),
            (double) generateInt(minCoord, maxCoord, coordStep),
            (double) generateInt(minCoord, maxCoord, coordStep),
            (double) generateInt(minSpeed, maxSpeed, speedStep),
            (double) generateInt(minSpeed, maxSpeed, speedStep)
        );
    }

    fclose(fp);
}

/* Формируем входные данные.
Рассматриваемое количество точек: 64, 128, 256, 512, 1024. */
int main(){
    int bodies, timeSteps;
    srand(time(NULL));
    for (bodies = 64; bodies <= 1024; bodies *= 2){
        generateInput(bodies);
    }
    return 0;
}
