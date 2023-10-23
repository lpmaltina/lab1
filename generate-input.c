#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generateInt(int min, int max, int step){
    return min + rand() % ((max - min) / step + 1) * step;
}

void generateInput(int bodies, int timeSteps){
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
    sprintf(fileName, "input/input-%d-%d", bodies, timeSteps);

    FILE *fp = fopen(fileName, "w");
    fprintf(fp, "6.6743e-11 %d %d\n", bodies, timeSteps);

    for (int i = 0; i < bodies; ++i){
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

int main(){
    srand(time(NULL));
    for (int bodies = 64; bodies <= 1024; bodies *= 2){
        for (int timeSteps = 10; timeSteps <= 1000; timeSteps *= 10){
            generateInput(bodies, timeSteps);
        }
    }
    return 0;
}
