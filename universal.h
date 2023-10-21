#ifndef __UNIVERSAL_H__
#define __UNIVERSAL_H__


#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define DT 0.05
#define eps 0.00000001


struct vec {
	double x;
	double y;
};
const struct vec vecEmpty = {0};


struct vec addVecs(struct vec a, struct vec b)
{
	struct vec c = { a.x + b.x, a.y + b.y };
	return c;
}


struct vec subVecs(struct vec a, struct vec b)
{
	struct vec c = { a.x - b.x, a.y - b.y };
	return c;
}


struct vec scaleVec(double b, struct vec a)
{
	struct vec c = { a.x * b, a.y * b };
	return c;
}


double modVec(struct vec a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}


int bodies = 0;
int timeSteps = 0;
double* masses = NULL;
double GravConstant = 0.;

FILE *fIn = NULL;
FILE *fOut = NULL;

struct vec *poses = NULL;
struct vec *velocities = NULL;
struct vec *accelerations = NULL;


int initiateSystem(char *str_fIn, char *str_fOut)
{
    int i;
    FILE *fp = fopen(str_fIn, "r");
    fscanf(fp, "%lf%d%d", &GravConstant, &bodies, &timeSteps);

    masses = (double *)malloc(bodies * sizeof(double));
    poses = (struct vec *)malloc(bodies * sizeof(struct vec));
    velocities = (struct vec *)malloc(bodies * sizeof(struct vec));
    accelerations = (struct vec *)malloc(bodies * sizeof(struct vec));

    for (i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &poses[i].x, &poses[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);

	fOut = fopen(str_fOut, "w+");
	if (fOut == NULL) {
		printf("err: cannot open destination file\n");
		return 1;
	}
	return 0;
}


/* TODO get rid of typedef */
typedef struct vec vec;


#endif /*__UNIVERSAL_H__*/
