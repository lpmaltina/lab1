/* Код для работы с двумерными векторами */
#include "vec.h"

vec addVectors(vec a, vec b){
    vec c = {a.x + b.x, a.y + b.y};
    return c;
}

vec scaleVector(double b, vec a){
    vec c = {b * a.x, b * a.y};
    return c;
}

vec subtractVectors(vec a, vec b){
    vec c = {a.x - b.x, a.y - b.y};
    return c;
}

double modVector(vec a){
    return sqrt(a.x * a.x + a.y * a.y);
}
