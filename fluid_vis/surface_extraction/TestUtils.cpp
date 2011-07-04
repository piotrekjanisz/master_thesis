#include "TestUtils.h"
#include <cstdlib>

const float TestUtils::FLOAT_PRECISION = 0.0000001f;


inline float getRandFloat(float min, float max)
{
	return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

void TestUtils::generateVectors(float* v, int count, int components, float min, float max, int seed)
{
	srand(seed);

	for (int i = 0; i < count; i++) {
		v[i*components + 0] = getRandFloat(min, max);
		v[i*components + 1] = getRandFloat(min, max);
		v[i*components + 2] = getRandFloat(min, max);
	}
}