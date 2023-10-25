#include "universal.h"


void computeAccels()
{
    int i = 0;
	int j = 0;
	double denom = 0.;

	vec vec_pose_diff = {0};
	vec vec_without_mass = {0};
	vec vec_acc_i = {0};
	vec vec_acc_j = {0};
	vec vec_acc_ij = {0};
	vec vec_acc_ji = {0};

    for (i = 0; i < bodies; i++) {
		vec_acc_i = accels[i];
        for (j = i + 1; j < bodies; j++) {
			vec_pose_diff = subVecs(poses[j], poses[i]);
			denom = pow(modVec(vec_pose_diff), 3);
			/* if (denom < eps){ */
				/* denom = eps; */
			/* } */
			vec_without_mass = scaleVec(GravConst / denom, vec_pose_diff);
			vec_acc_j = accels[j];
			vec_acc_ij = scaleVec(masses[j], vec_without_mass);
			vec_acc_ji = scaleVec(-masses[i], vec_without_mass);
			vec_acc_i = addVecs(vec_acc_i, vec_acc_ij);
			vec_acc_j = addVecs(vec_acc_j, vec_acc_ji);
			accels[j] = vec_acc_j;
        }
		accels[i] = vec_acc_i;
    }
}

void* routine()
{
	int i = 0;
	int j = 0;

	for (i = 0; i < 1; i++) {
		
		for (j = 0; j < bodies; j++) {
			accels[j] = vecEmpty;
		}

		computeAccels();
		fprintf(fOut, "%d\t", i + 1);
		for (j = 0; j < bodies; j++) {
			fprintf(fOut, "%.20lf\t%.20lf\t", accels[j].x, accels[j].y);
		}
		fprintf(fOut, "\n");
	}

	return NULL;
}

int main()
{
	rslt = initiateSystem(
		"input/input-10-10",
		"output/output-serial-10-10");
	if (rslt != 0) { return 1; }

	routine();

	terminateSystem();

    return 0;
}
