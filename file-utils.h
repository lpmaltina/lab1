/* Код для чтения из файлов директории input и
записи в файлы директории output */
#ifndef FILE_UTILS
#define FILE_UTILS

#include <stdlib.h>
#include <stdio.h>
#include "vec.h"

#define DELIMITER ','

extern int bodies;
extern double *masses;
extern vec *positions, *velocities, *accelerations;

/* Функция, которая считывает входные данные из файла с именем fileName.
Считывает из файла количество точек bodies.

Выделяет память под глобальные массивы:
- masses
- positions
- velocities
- accelerations

Заполняет прочитанными значениями глобальные массивы: 
- masses
- positions
- velocities */
void initiateSystem(char *fileName);

/* Функция, которая записывает в файл с именем fileName
координаты точек на временном шаге с номером timeStep в формате:
<шаг><разделитель><x1><разделитель><y1><разделитель> ... <xn><разделитель><yn>
*/
void writeTimeStepInfo(FILE* fileName, int timeStep);

/* Функция, которая записывает в файл с именем fileName
заголовок в формате:
x1<разделитель>y1<разделитель> ... xn<разделитель>yn
*/
void writeHeader(FILE* fileName);

#endif //FILE_UTILS
