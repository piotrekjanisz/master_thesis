#include "TestUtils.h"
#include <surface_extraction\ParticleLookupCache.h>

#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <cstring>
#include <iostream>

struct ParticleLookupCachePerformanceTestFixture
{
	ParticleLookupCachePerformanceTestFixture() {}
	~ParticleLookupCachePerformanceTestFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(ParticleLookupCachePerformanceTest, ParticleLookupCachePerformanceTestFixture);

BOOST_AUTO_TEST_CASE(getFieldValueAtTest_100x100x100_3000part)
{
	const int SIZE = 100;
	const int PARTICLE_COUNT = 3000;
	const int LOOKUP_COUNT = 100000;
	const int PARTICLE_COMPONENTS = 3;
	const double RC = 1.0;
	const double CUBE_SIZE = 0.5;

	float* particles;
	ArrayHandle<float> handle1(particles = new float[PARTICLE_COUNT * PARTICLE_COMPONENTS]);
	TestUtils::generateVectors(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, 0, 50, 1);

	float** particlePtrs;
	ArrayHandle<float*> handle2(particlePtrs = new float*[PARTICLE_COUNT]);

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		particlePtrs[i] = &particles[i * PARTICLE_COMPONENTS];
	}

	ParticleLookupCache cache(SIZE, SIZE, SIZE);
	cache.init(particlePtrs, PARTICLE_COUNT, PARTICLE_COMPONENTS, RC, CUBE_SIZE);

	double sum = 0.0;
	MEASURE_TIME("getFieldValueAtTest_100x100x100_3000part",
		for (int z = 0; z < SIZE; z++) {
			for (int y = 0; y < SIZE; y++) {
				for (int x = 0; x < SIZE; x++) {
					sum += cache.getFieldValueAt(x, y, z);
				}
			}
		}
	)
}

BOOST_AUTO_TEST_CASE(getFieldValueAtTest_100x100x100_3000part_1000000lookups)
{
	const int SIZE = 100;
	const int PARTICLE_COUNT = 3000;
	const int LOOKUP_COUNT = 1000000;
	const int PARTICLE_COMPONENTS = 3;
	const double RC = 1.0;
	const double CUBE_SIZE = 0.5;

	float* particles;
	ArrayHandle<float> handle1(particles = new float[PARTICLE_COUNT * PARTICLE_COMPONENTS]);
	TestUtils::generateVectors(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, 0, 50, 1);

	float** particlePtrs;
	ArrayHandle<float*> handle2(particlePtrs = new float*[PARTICLE_COUNT]);

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		particlePtrs[i] = &particles[i * PARTICLE_COMPONENTS];
	}

	ParticleLookupCache cache(SIZE, SIZE, SIZE);
	cache.init(particlePtrs, PARTICLE_COUNT, PARTICLE_COMPONENTS, RC, CUBE_SIZE);

	double sum = 0.0;
	int x = 0;
	MEASURE_TIME("getFieldValueAtTest_100x100x100_1000part_1000000lookups",
		for (int i = 0; i < LOOKUP_COUNT; i++) {
			sum += cache.getFieldValueAt(x, 50, 90);
			x = (x + 1) % SIZE;
		}
	)
}

BOOST_AUTO_TEST_CASE(initTest_100x100x100_3000part)
{
	const int SIZE = 100;
	const int PARTICLE_COUNT = 3000;
	const int LOOKUP_COUNT = 1000000;
	const int PARTICLE_COMPONENTS = 3;
	const double RC = 1.0;
	const double CUBE_SIZE = 0.5;
	
	float* particles;
	ArrayHandle<float> handle1(particles = new float[PARTICLE_COUNT * PARTICLE_COMPONENTS]);
	TestUtils::generateVectors(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, 0, 50, 1);

	float** particlePtrs;
	ArrayHandle<float*> handle2(particlePtrs = new float*[PARTICLE_COUNT]);

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		particlePtrs[i] = &particles[i * PARTICLE_COMPONENTS];
	}

	ParticleLookupCache cache(SIZE, SIZE, SIZE);

	MEASURE_TIME("initTest_100x100x100_3000part",
		cache.init(particlePtrs, PARTICLE_COUNT, PARTICLE_COMPONENTS, RC, CUBE_SIZE);
	)
}

BOOST_AUTO_TEST_SUITE_END();