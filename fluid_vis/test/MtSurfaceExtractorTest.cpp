#include <surface_extraction\MtSurfaceExtractor.h>
#include <boost/test/unit_test.hpp>
#include <list>

struct MtSurfaceExtractorTestFixture 
{
	MtSurfaceExtractorTestFixture()
	{
		desc.xMin = -10.0;
		desc.xMax =  10.0;
		desc.yMin = -10.0;
		desc.yMax =  10.0;
		desc.zMin = -10.0;
		desc.zMax =  10.0;

		desc.blockSize = 5.0;
		desc.cubeSize = 0.1;

		desc.rc = 0.3;

		desc.maxParticles = 1000;

		desc.isoTreshold = 0.2;

		desc.threads = 1;
	}

	~MtSurfaceExtractorTestFixture()
	{
	}

	SurfaceExtractorDesc desc;
};

BOOST_FIXTURE_TEST_SUITE(MtSurfaceExtractorTest, MtSurfaceExtractorTestFixture);

BOOST_AUTO_TEST_CASE(initTest)
{
	MtSurfaceExtractor extractor(desc);
}

BOOST_AUTO_TEST_CASE(extractSurfaceTest)
{
	MtSurfaceExtractor extractor(desc);
	const int PARTICLES_COUNT = 3;
	const int PARTICLES_COMPONENTS = 3;
	float particles[PARTICLES_COUNT * PARTICLES_COMPONENTS] = {
		 0.0f,  0.0f,  0.0f,
		 7.5f,  7.5f,  7.5f,
		 1.0f,  1.0f,  1.0f
	};

	std::list<TriangleMesh> output;

	extractor.extractSurface(particles, PARTICLES_COUNT, PARTICLES_COMPONENTS, output);
}

BOOST_AUTO_TEST_SUITE_END();