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

pthread_barrier_t barrier;
int thread_count = 5;
pthread_mutex_t mutex, barrier_mutex;
FILE* outputFile;
int bodies, timeSteps;
double *masses, GravConstant;
vector *positions, *velocities, *accelerations;
vector *next_positions, *next_velocities;
pthread_cond_t cond_var;
int counter = 0;


// void* barrier(void* rank)
// {

//     pthread_cond_init(&cond_var, NULL);
//     pthread_mutex_init(&barrier_mutex, NULL);

//     pthread_mutex_lock(&barrier_mutex);
//     counter++;
//     if(counter == thread_count)
//     {
//         counter = 0;
//         pthread_cond_broadcast(&cond_var);
//     }
//     else
//     {
//         while(pthread_cond_wait(&cond_var, &barrier_mutex) != 0);
//     }
//     pthread_mutex_unlock(&barrier_mutex);

//     pthread_cond_destroy(&cond_var);
//     pthread_mutex_destroy(&barrier_mutex);
// }


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
    next_positions = (vector *)malloc(bodies * sizeof(vector));
    next_velocities = (vector *)malloc(bodies * sizeof(vector));

    for (i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}


void computeAccelerations(int start, int finish)
{
    int i, j;
    for (i = start; i < finish; i++)
    {
        vector accelerationI = accelerations[i];
        for (j = 0; j < bodies; j++)
        {
            if (i != j)
            {
                vector positionsDiff = subtractVectors(positions[j], positions[i]);
                double denominator = pow(mod(positionsDiff), 3);
                if (denominator < eps){
                    denominator = eps;
                }
                vector withoutMass = scaleVector(GravConstant / denominator, positionsDiff);
                vector accelerationIJ = scaleVector(masses[j], withoutMass);
                accelerationI = addVectors(accelerationI, accelerationIJ);
               
            }
        }
        accelerations[i] = accelerationI;
    }
}


void computeVelocities(int start, int finish)
{
    int i;
    for (i = start; i < finish; i++)
        next_velocities[i] = addVectors(velocities[i], scaleVector(DT, accelerations[i]));
}


void computePositions(int start, int finish)
{
    int i;
    for (i = start; i < finish; i++)
        next_positions[i] = addVectors(positions[i], scaleVector(DT, velocities[i]));
}

void updateArrays(int start, int finish){
    for (int i = start; i < finish; ++i){
        positions[i] = next_positions[i];
        velocities[i] = next_velocities[i];
    }
}

void* routine(void* nthread){
    long long my_nthread = (long long) nthread;
    int my_points = bodies / thread_count;
    int my_first_point = my_nthread * my_points;
    int my_last_point = my_first_point + my_points;
    
     for (int i = 0; i < timeSteps; ++i){
        for (int j = my_first_point; j < my_last_point; ++j){
            accelerations[j].x = 0;
            accelerations[j].y = 0;
        }
        computeAccelerations(my_first_point, my_last_point);
        computePositions(my_first_point, my_last_point); 
        computeVelocities(my_first_point, my_last_point);
        pthread_barrier_wait(&barrier);
        updateArrays(my_first_point, my_last_point);
        pthread_barrier_wait(&barrier);
        if (my_nthread == 0){
            fprintf(outputFile, "%d\t", i + 1);
            for (int j = 0; j < bodies; j++){
                fprintf(outputFile, "%f\t%f\t", positions[j].x, positions[j].y);
            }
            fprintf(outputFile, "\n");
        }
        pthread_barrier_wait(&barrier);
    }
    return NULL;
}


int main()
{
    int i, j;
    int timeStep = 0;
    initiateSystem("input/input-10-1000");
    outputFile = fopen("output/output-parallel-10-1000", "a");
    fprintf(outputFile, "t\t");
    for (j = 1; j < bodies + 1; ++j){
        fprintf(outputFile, "x%d\ty%d\t", j, j);
    }
    fprintf(outputFile, "\n");
    
    pthread_barrier_init(&barrier, NULL, thread_count);

    pthread_t* pthread_handles = malloc(thread_count * sizeof(pthread_t));
    for (long long i = 0; i < thread_count; ++i){
        pthread_create(&pthread_handles[i], NULL, routine, (void*) i);
    }

    for (long long i = 0; i < thread_count; ++i){
        pthread_join(pthread_handles[i], NULL);
    }
    free(pthread_handles);
    
    pthread_barrier_destroy(&barrier);
    fclose(outputFile);
    free(masses);
    free(accelerations);
    free(velocities);
    free(positions);
    free(next_positions);
    free(next_velocities);
    return 0;
}