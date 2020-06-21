#include "MWPutil.h"

static int seed1 = 333;
static int seed2 = 42;
static int seed3 = 1800;


void setMWPSeeds(int s1, int s2, int s3)
{
	seed1 = s1;
	seed2 = s2;
	seed3 = s3;
}


double MWPrand()
{
	seed1 = (171 * seed1) % 30269;
	seed2 = (172 * seed2) % 30307;
	seed3 = (170 * seed3) % 30323;

	return fmod((seed1 / 30269.0) + (seed2 / 30307.0) + (seed3 / 30323.0),1);
}