/* Код для работы с двумерными векторами */
#ifndef VEC
#define VEC

#include <math.h>

typedef struct{
    double x, y;
} vec;

/* Функция, которая возвращает сумму векторов a и b. */
vec addVectors(vec a, vec b);

/* Функция, которая возвращает произведение числа b и вектора a. */
vec scaleVector(double b, vec a);

/* Функция, которая возвращает разность векторов a и b. */
vec subtractVectors(vec a, vec b);

/* Функция, которая возвращает модуль вектора a. */
double modVector(vec a);

#endif //VEC