#include "TestUtils.h"
#include <surface_extraction\ParticleLookupCache.h>
#include <surface_extraction\SurfaceExtractor.h>
#include <fluid_vis\debug_utils.h>

#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <cstring>
#include <iostream>

struct SurfaceExtractorPerformanceTestFixture 
{
	SurfaceExtractorPerformanceTestFixture()
	{
	}

	~SurfaceExtractorPerformanceTestFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(SurfaceExtractorPerformanceTest, SurfaceExtractorPerformanceTestFixture);

BOOST_AUTO_TEST_CASE(extractSurfacePerformanceTest_1000p_100x100x100)
{
	
	const int PARTICLE_COUNT = 1000;
	const int MAX_VERTICES = 1000000;
	const int MAX_TRIANGLES = 1000000;
	const int PARTICLE_COMPONENTS = 3;
	const int VERT_COMPONENTS = 4;
	float* particles = new float[PARTICLE_COUNT * PARTICLE_COMPONENTS];
	TestUtils::generateVectors(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, 0, 50.0, 1);

	float* verticesBuffer = new float[MAX_VERTICES * VERT_COMPONENTS];
	float* normalBuffer = new float[MAX_VERTICES * 3];
	unsigned int* indicesBuffer = new unsigned int[MAX_TRIANGLES * 3];

	ArrayHandle<float> handle1(particles);
	ArrayHandle<float> handle2(verticesBuffer);
	ArrayHandle<float> handle3(normalBuffer);
	ArrayHandle<unsigned int> handle4(indicesBuffer);

	SurfaceExtractor surfaceExtractor(0.0, 50.0, 0.0, 50.0, 0.0, 50.0, 1.5, 0.5, 0.2);

	int nvert;
	int ntriag;
	MEASURE_TIME("extractSurfacePerformanceTest_1000p_100x100x100", 
		surfaceExtractor.extractSurface(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, VERT_COMPONENTS, verticesBuffer, normalBuffer, indicesBuffer, nvert, ntriag, MAX_VERTICES, MAX_TRIANGLES);
	)
	std::cout << "\tntriag: " << ntriag << std::endl;
	std::cout << "\tnvert: " << nvert << std::endl;
	TestUtils::printArray(indicesBuffer, 3, 10);
}

BOOST_AUTO_TEST_SUITE_END();