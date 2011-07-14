#include "TestUtils.h"
#include <surface_extraction\MtSurfaceExtractor.h>
#include <fluid_vis\debug_utils.h>

#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <cstring>
#include <iostream>

struct MtSurfaceExtractorPerformanceTestFixture 
{
	MtSurfaceExtractorPerformanceTestFixture()
	{
		desc.xMin = -5.0;
		desc.xMax =  5.0;
		desc.yMin = -5.0;
		desc.yMax =  5.0;
		desc.zMin = -5.0;
		desc.zMax =  5.0;
		desc.isoTreshold = 0.2;
		desc.blockSize = 5.0;
		desc.cubeSize = 0.05;
		desc.rc = 0.15;
		desc.maxParticles = 3000;
		desc.threads = 2;
		desc.maxTrianglesPerThread = 500000;
		desc.maxVerticesPerThread = 500000;
	}

	~MtSurfaceExtractorPerformanceTestFixture()
	{
	}

	void testPerformance(int nthreads, int particlesCount, const char* title)
	{
		const int PARTICLE_COUNT = particlesCount;
		const int PARTICLE_COMPONENTS = 3;
		const int VERT_COMPONENTS = 4;
		float* particles;
		ArrayHandle<float> handle1(particles = new float[PARTICLE_COUNT * PARTICLE_COMPONENTS]);
		TestUtils::generateVectors(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, -4.0f, 4.0f, 1);
		desc.threads = nthreads;
		MtSurfaceExtractor surfaceExtractor(desc);

		std::list<TriangleMesh> output;
		MEASURE_TIME_AVG(title, 10,
			surfaceExtractor.extractSurface(particles, PARTICLE_COUNT, PARTICLE_COMPONENTS, &output);
			surfaceExtractor.waitForResults();
		)
	}

	SurfaceExtractorDesc desc;
};

BOOST_FIXTURE_TEST_SUITE(MtSurfaceExtractorPerformanceTest, MtSurfaceExtractorPerformanceTestFixture);

BOOST_AUTO_TEST_CASE(extractSurfacePerformanceTest_1000p_200x200x200)
{
	std::cout << "--- 1000 particles, 200x200x200 cubes, 2x2x2 blocks ---" << std::endl;
	testPerformance(1, 1000, "1 thread");
	testPerformance(2, 1000, "2 thread");
	testPerformance(3, 1000, "3 thread");
	testPerformance(4, 1000, "4 thread");
	testPerformance(5, 1000, "5 thread");
}

BOOST_AUTO_TEST_CASE(extractSurfacePerformanceTest_3000p_200x200x200)
{
	std::cout << "--- 3000 particles, 200x200x200 cubes, 2x2x2 blocks ---" << std::endl;
	testPerformance(1, 3000, "1 thread");
	testPerformance(2, 3000, "2 thread");
	testPerformance(3, 3000, "3 thread");
	testPerformance(4, 3000, "4 thread");
	testPerformance(5, 3000, "5 thread");
}

BOOST_AUTO_TEST_SUITE_END();