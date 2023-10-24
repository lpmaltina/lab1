/* Код для чтения из файлов директории input и
записи в файлы директории output */
#include "file-utils.h"

int bodies;
double *masses;
vec *positions, *velocities, *accelerations;

void initiateSystem(char *fileName){
    int i;
    FILE *fp = fopen(fileName, "r");

    fscanf(fp, "%d", &bodies);
    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vec *)malloc(bodies * sizeof(vec));
    velocities = (vec *)malloc(bodies * sizeof(vec));
    accelerations = (vec *)malloc(bodies * sizeof(vec));

    for (i = 0; i < bodies; ++i){
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}

void writeTimeStepInfo(FILE* fileName, int timeStep){
    int body;
    fprintf(fileName, "%d%c", timeStep, DELIMITER);
    for (body = 0; body < bodies; ++body){
        fprintf(
            fileName,
            "%f%c%f%c",
            positions[body].x,
            DELIMITER,
            positions[body].y,
            DELIMITER
        );
    }
    fprintf(fileName, "\n");
}

void writeHeader(FILE* fileName){
    int body;
    fprintf(fileName, "t%c", DELIMITER);
    for (body = 1; body < bodies + 1; ++body){
        fprintf(fileName, "x%d%cy%d%c", body, DELIMITER, body, DELIMITER);
        }
    fprintf(fileName, "\n");
}
