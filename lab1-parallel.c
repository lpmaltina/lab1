#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>


#define DT 0.05
#define eps 0.00000001

typedef struct
{
    double x, y;
} vector;


int thread_count = 5;
pthread_mutex_t mutex, barrier_mutex;
FILE* outputFile;
int bodies, timeSteps;
double *masses, GravConstant;
vector *positions, *velocities, *accelerations;
pthread_cond_t cond_var;
int counter = 0;


void* barrier(void* rank)
{

    pthread_cond_init(&cond_var, NULL);
    pthread_mutex_init(&barrier_mutex, NULL);

    pthread_mutex_lock(&barrier_mutex);
    counter++;
    if(counter == thread_count)
    {
        counter = 0;
        pthread_cond_broadcast(&cond_var);
    }
    else
    {
        while(pthread_cond_wait(&cond_var, &barrier_mutex) != 0);
    }
    pthread_mutex_unlock(&barrier_mutex);

    pthread_cond_destroy(&cond_var);
    pthread_mutex_destroy(&barrier_mutex);
}


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


int isCollision(vector point1, vector point2){
    return (
        fabs(point1.x - point2.x) < eps && 
        fabs(point1.y - point2.y) < eps
    );
}


void resolveCollisions()
{
    int i, j;
    for (i = 0; i < bodies - 1; i++)
        for (j = i + 1; j < bodies; j++)
        {
            if (isCollision(positions[i], positions[j])){
                vector temp = velocities[i];
                velocities[i] = velocities[j];
                velocities[j] = temp;
            }
        }
}


void computeAccelerations(int start, int finish)
{
    int i, j;
    for (i = start; i < finish; i++)
    {
        vector accelerationI = accelerations[i];
        for (j = i + 1; j < bodies; j++)
        {
            vector positionsDiff = subtractVectors(positions[j], positions[i]);
            vector withoutMass = scaleVector(GravConstant / pow(mod(positionsDiff), 3), positionsDiff);
            vector changeAccelerationI = scaleVector(masses[j], withoutMass);
            vector changeAccelerationJ = scaleVector(masses[i], withoutMass);
            accelerationI = addVectors(accelerationI, changeAccelerationI);
            vector accelerationJ = subtractVectors(accelerations[j], changeAccelerationJ);
            
            pthread_mutex_lock(&mutex);
            accelerations[j] = accelerationJ;
            pthread_mutex_unlock(&mutex);
        }
        
        pthread_mutex_lock(&mutex);
        accelerations[i] = accelerationI;
        pthread_mutex_unlock(&mutex);
    }
}


void computeVelocities(int start, int finish)
{
    int i;
    for (i = start; i < finish; i++)
        velocities[i] = addVectors(velocities[i], scaleVector(DT, accelerations[i]));
}


void computePositions(int start, int finish)
{
    int i;
    for (i = start; i < finish; i++)
        positions[i] = addVectors(positions[i], scaleVector(DT, velocities[i]));
}


void* routine(void* nthread){
    long long my_nthread = (long long) nthread;
    int my_points = bodies / thread_count;
    int my_first_point = my_nthread * my_points;
    int my_last_point = my_first_point + my_points;
    computeAccelerations(my_first_point, my_last_point);
    computePositions(my_first_point, my_last_point);
    computeVelocities(my_first_point, my_last_point);
    return NULL;
}


int main()
{
    int i, j;
    int timeStep = 0;
    initiateSystem("input/input-10-10");
    outputFile = fopen("output/output-parallel-10-10", "a");
    fprintf(outputFile, "t\t");
    for (j = 1; j < bodies + 1; ++j){
        fprintf(outputFile, "x%d\ty%d\t", j, j);
    }
    fprintf(outputFile, "\n");
        
    for (i = 0; i < bodies; ++i){
        accelerations[i].x = 0;
        accelerations[i].y = 0;
    }

    pthread_t* pthread_handles = malloc(thread_count * sizeof(pthread_t));
    for (long long i = 0; i < thread_count; ++i){
        pthread_create(&pthread_handles[i], NULL, routine, (void*) i);
    }

    for (long long i = 0; i < thread_count; ++i){
        pthread_join(pthread_handles[i], NULL);
    }

    resolveCollisions();
    fprintf(outputFile, "%d\t", timeStep + 1);
    for (j = 0; j < bodies; j++){
        fprintf(outputFile, "%f\t%f\t", positions[j].x, positions[j].y);
    }
    fprintf(outputFile, "\n");

    fclose(outputFile);
    free(pthread_handles);
    free(masses);
    free(accelerations);
    free(velocities);
    free(positions);
    return 0;
}
