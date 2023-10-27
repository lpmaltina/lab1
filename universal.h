#ifndef __UNIVERSAL_H__
#define __UNIVERSAL_H__


#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define DT 0.05
#define EPS 0.00000001


#define FILE_INPUT "input/input-10-10"
#define FILE_OUTPUT "output/output-10-10" /* WARN! might be redefined later */


struct vec {
	long double x;
	long double y;
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

struct vec scaleVec(long double b, struct vec a)
{
	struct vec c = { a.x * b, a.y * b };
	return c;
}

long double modVec(struct vec a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}


int bodies = 0;
int timeSteps = 0;
long double* masses = NULL;
long double GravConst = 0.;

FILE *fOut = NULL;

int rslt = 0;

struct vec *accels = NULL;
struct vec *poses = NULL;
struct vec *vels = NULL;


int initiateSystem(char *str_fIn, char *str_fOut)
{
	int i;
	int j;

	FILE *fIn = NULL;

    fIn = fopen(str_fIn, "r");
	if (fIn == NULL) {
		printf("err: cannot open input file\n");
		return 1;
	}
    fscanf(fIn, "%Lf%d%d", &GravConst, &bodies, &timeSteps);

    masses = calloc(bodies, sizeof(long double));
    accels = calloc(bodies, sizeof(struct vec));
    poses = calloc(bodies, sizeof(struct vec));
    vels = calloc(bodies, sizeof(struct vec));

    for (i = 0; i < bodies; i++)
    {
        fscanf(fIn, "%Lf", &masses[i]);
        fscanf(fIn, "%Lf%Lf", &poses[i].x, &poses[i].y);
        fscanf(fIn, "%Lf%Lf", &vels[i].x, &vels[i].y);
    }

    fclose(fIn);

	fOut = fopen(str_fOut, "w+");
	if (fOut == NULL) {
		printf("err: cannot open output file\n");
		return 1;
	}

    /* fprintf(fOut, "t\t"); */
    /* for (j = 1; j < bodies + 1; ++j){ */
    /*     fprintf(fOut, "x%d\ty%d\t", j, j); */
    /* } */
    /* fprintf(fOut, "\n"); */

	return 0;
}

int terminateSystem()
{
	fclose(fOut);
	free(masses);
    free(accels);
    free(vels);
    free(poses);
	return 0;
}


/* TODO get rid of typedef */
typedef struct vec vec;


#endif /*__UNIVERSAL_H__*/
