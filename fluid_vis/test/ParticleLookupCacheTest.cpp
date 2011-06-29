#define DEBUG

#include "TestUtils.h"
#include <boost/test/unit_test.hpp>
#include <surface_extraction\ParticleLookupCache.h>
#include <fluid_vis\debug_utils.h>


using namespace std;

struct ParticleLookupCacheTestFixture
{
	ParticleLookupCacheTestFixture()
		: cache(100, 100, 100)
	{

	}

	~ParticleLookupCacheTestFixture()
	{

	}

	ParticleLookupCache cache;
};


BOOST_FIXTURE_TEST_SUITE(ParticleLookupCacheTest, ParticleLookupCacheTestFixture);


BOOST_AUTO_TEST_CASE(create)
{
	float particles[] = {
		1.0, 1.0, 1.0,
		2.0, 2.0, 2.0
	};

	float* particlePtrs[] = { &particles[0], &particles[3] };

	cache.init(particlePtrs, 2, 3, 1.0, 0.5);
}

BOOST_AUTO_TEST_CASE(getFieldValueAt)
{
	float particles[] = {
		1.0, 1.0, 1.0,
		2.0, 2.0, 2.0
	};

	float* particlePtrs[] = { &particles[0], &particles[3] };

	cache.init(particlePtrs, 2, 3, 1.0, 0.5);

	BOOST_CHECK_CLOSE_FRACTION(0.25, cache.getFieldValueAt(2, 2, 2), TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(0.25, cache.getFieldValueAt(4, 4, 4), TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(getFieldValueAt_checkZero)
{
	float particles[] = {
		3.0, 3.0, 3.0,
	};

	float* particlePtrs[] = { 
		&particles[0], 
	};

	cache.init(particlePtrs, 1, 3, 1.0, 0.5);

	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(6, 6, 4));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(6, 4, 6));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(4, 6, 6));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(6, 6, 8));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(6, 8, 6));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(8, 6, 6));

	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(4, 4, 6));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(4, 8, 6));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(8, 4, 6));
	BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(8, 8, 6));
}


BOOST_AUTO_TEST_CASE(getFieldValueAt_checkNonZero)
{
	float particles[] = {
		3.0, 3.0, 3.0,
	};

	float* particlePtrs[] = { 
		&particles[0], 
	};

	cache.init(particlePtrs, 1, 3, 1.0, 0.5);

	BOOST_CHECK_GT(cache.getFieldValueAt(5, 5, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 5, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(7, 5, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(5, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(7, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(5, 7, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 7, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(7, 7, 6), 0.0);

	BOOST_CHECK_GT(cache.getFieldValueAt(5, 6, 5), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(5, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(5, 6, 7), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 5), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 7), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(7, 6, 5), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(7, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(7, 6, 7), 0.0);

	BOOST_CHECK_GT(cache.getFieldValueAt(6, 5, 5), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 5), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 7, 5), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 5, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 7, 6), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 5, 7), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 6, 7), 0.0);
	BOOST_CHECK_GT(cache.getFieldValueAt(6, 7, 7), 0.0);
}

BOOST_AUTO_TEST_CASE(getFieldValueAt_checkDouble)
{
	float particles[] = {
		3.0, 3.0, 3.0,
		3.0, 3.0, 3.0,
	};

	float* particlePtrs[] = { 
		&particles[0], 
		&particles[3],
	};

	cache.init(particlePtrs, 2, 3, 1.0, 0.5);

	BOOST_CHECK_CLOSE_FRACTION(0.5, cache.getFieldValueAt(6, 6, 6), TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(getFieldValueAt_cube)
{
	float particles[] = {
		3.0, 3.0, 3.0,
		3.0, 4.0, 3.0,
		4.0, 3.0, 3.0,
		4.0, 4.0, 3.0,
		3.0, 3.0, 4.0,
		3.0, 4.0, 4.0,
		4.0, 3.0, 4.0,
		4.0, 4.0, 4.0,
	};

	float* particlePtrs[] = { 
		&particles[0], 
		&particles[3], 
		&particles[6], 
		&particles[9], 
		&particles[12], 
		&particles[15], 
		&particles[18], 
		&particles[21], 
	};

	cache.init(particlePtrs, 8, 3, 1.0, 0.5);
	
	for (int i = 0; i < 7; i++) {
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(4+i, 4, 6));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(4, 4+i, 6));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(4+i, 10, 6));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(10, 4+i, 6));
	}	
}

BOOST_AUTO_TEST_CASE(getFieldValueAt_boundary)
{
	float particles[] = {
		3.0, 3.0, 0.5,
	};

	float* particlePtrs[] = { 
		&particles[0], 
	};

	cache.init(particlePtrs, 1, 3, 1.0, 0.5);

	BOOST_CHECK_CLOSE_FRACTION(0.25, cache.getFieldValueAt(6, 6, 1), TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(0.0, cache.getFieldValueAt(6, 6, 0), TestUtils::FLOAT_PRECISION);
}


BOOST_AUTO_TEST_CASE(getFieldValueAtTest_1000part_zeroOnBoundary)
{
	const int PARTICLE_COUNT = 1000;
	const int PARTICLE_COMPONENTS = 3;
	float* particles;
	ArrayHandle<float> handle1(particles = new float[PARTICLE_COUNT * PARTICLE_COMPONENTS]);
	float** particlePtrs;
	ArrayHandle<float*> handle2(particlePtrs = new float*[PARTICLE_COUNT]);

	TestUtils::generateVectors(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, 50.0f, 50.0f, 1);
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		particlePtrs[i] = &particles[i * PARTICLE_COMPONENTS];
	}

	cache.init(particlePtrs, PARTICLE_COUNT, PARTICLE_COMPONENTS, 1.0, 0.5);
	for (int i = 0; i < 100; i++) {
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(0, 0, i));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(0, i, 0));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(0, i, i));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(i, 0, 0));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(i, 0, i));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(i, i, 0));
		BOOST_CHECK_EQUAL(0.0, cache.getFieldValueAt(i, i, i));
	}
}

BOOST_AUTO_TEST_SUITE_END();