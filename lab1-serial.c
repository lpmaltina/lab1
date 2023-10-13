#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "timer.h"

#define DT 0.05
#define eps 0.00000001

typedef struct
{
    double x, y;
} vector;

double start = 0;
double end = 0;
int bodies, timeSteps;
double *masses, GravConstant;
vector *positions, *velocities, *accelerations;

vector addVectors(vector a, vector b)
{
    vector c = {a.x + b.x, a.y + b.y};

    return c;
}

vector scaleVector(double b, vector a)
{
    vector c = {b * a.x, b * a.y};

    return c;
}

vector subtractVectors(vector a, vector b)
{
    vector c = {a.x - b.x, a.y - b.y};

    return c;
}

double mod(vector a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

int isCollision(vector point1, vector point2){
    return (
        fabs(point1.x - point2.x) < eps && 
        fabs(point1.y - point2.y) < eps
    );
}

void initiateSystem(char *fileName)
{
    int i;
    FILE *fp = fopen(fileName, "r");

    fscanf(fp, "%lf%d%d", &GravConstant, &bodies, &timeSteps);

    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vector *)malloc(bodies * sizeof(vector));
    velocities = (vector *)malloc(bodies * sizeof(vector));
    accelerations = (vector *)malloc(bodies * sizeof(vector));

    for (i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}

void computeAccelerations()
{
    int i, j;

    for (i = 0; i < bodies; i++)
    {
        accelerations[i].x = 0;
        accelerations[i].y = 0;
        for (j = 0; j < bodies; j++)
        {
            if (i != j)
            {
                double denominator = pow(mod(subtractVectors(positions[i], positions[j])), 3);
                if (denominator < eps){
                    denominator = eps;
                }
                accelerations[i] = addVectors(accelerations[i], scaleVector(GravConstant * masses[j] / denominator, subtractVectors(positions[j], positions[i])));
            }
        }
    }
}

void computeVelocities()
{
    int i;

    for (i = 0; i < bodies; i++)
        velocities[i] = addVectors(velocities[i], scaleVector(DT, accelerations[i]));
}

void computePositions()
{
    int i;

    for (i = 0; i < bodies; i++)
        positions[i] = addVectors(positions[i], scaleVector(DT,velocities[i]));
}

void simulate()
{
    computeAccelerations();
    computePositions();
    computeVelocities();
}

int main()
{
    int i, j;
    double time;
    int bodies, timeSteps;
    char inputFile[30] = {0};
    char outputFile[30] = {0};
    for (bodies = 10; bodies <= 1000; bodies = bodies * 10){
        for (timeSteps = 10; timeSteps <= 1000; timeSteps = timeSteps * 10){
            time = 0;
            sprintf(inputFile, "input/input-%d-%d", bodies, timeSteps);
            initiateSystem(inputFile);
            sprintf(outputFile, "output/output-serial-%d-%d", bodies, timeSteps);
            FILE *f = fopen(outputFile, "a");
            fprintf(f, "t\t");
            for (j = 1; j < bodies + 1; ++j){
                fprintf(f, "x%d\ty%d\t", j, j);
            }
            fprintf(f, "\n");

            for (i = 0; i < timeSteps; i++)
            {
                GET_TIME(start);
                simulate();
                GET_TIME(end);
                time += end - start;
                fprintf(f, "%d\t", i + 1);
                for (j = 0; j < bodies; j++){
                    fprintf(f, "%f\t%f\t", positions[j].x, positions[j].y);
                }
                fprintf(f, "\n");
            }
            fclose(f);
            printf("bodies=%d, timeSteps=%d: %f\n", bodies, timeSteps, time);
        }
    }
    free(masses);
    free(accelerations);
    free(velocities);
    free(positions);
    return 0;
}
