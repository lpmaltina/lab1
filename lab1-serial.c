#include "universal.h"


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
                double denominator = pow(modVec(subVecs(poses[i], poses[j])), 3);
                if (denominator < eps){
                    denominator = eps;
                }
                accelerations[i] = addVecs(accelerations[i], scaleVec(GravConstant * masses[j] / denominator, subVecs(poses[j], poses[i])));
            }
        }
    }
}

void computeVelocities()
{
    int i;

    for (i = 0; i < bodies; i++)
        velocities[i] = addVecs(velocities[i], scaleVec(DT, accelerations[i]));
}

void computePoses()
{
    int i;

    for (i = 0; i < bodies; i++)
        poses[i] = addVecs(poses[i], scaleVec(DT,velocities[i]));
}

void simulate()
{
    computeAccelerations();
    computePoses();
    computeVelocities();
}

int main()
{
	initiateSystem(
		"input/input-10-10",
		"output/output-serial-10-10");

	fprintf(fOut, "t\t");
	for (int j = 1; j < bodies + 1; ++j){
		fprintf(fOut, "x%d\ty%d\t", j, j);
	}
	fprintf(fOut, "\n");

	for (int i = 0; i < timeSteps; i++)
	{          
		simulate();
		fprintf(fOut, "%d\t", i + 1);
		for (int j = 0; j < bodies; j++){
			fprintf(fOut, "%.20f\t%.20f\t", poses[j].x, poses[j].y);
		}
		fprintf(fOut, "\n");
	}
	fclose(fOut);
	printf("bodies=%d, timeSteps=%d\n", bodies, timeSteps);

    free(masses);
    free(accelerations);
    free(velocities);
    free(poses);
    return 0;
}
