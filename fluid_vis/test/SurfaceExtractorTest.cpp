//#define DEBUG

#include "TestUtils.h"
#include <surface_extraction\ParticleLookupCache.h>
#include <surface_extraction\SurfaceExtractor.h>
#include <fluid_vis\debug_utils.h>

#include <boost/test/unit_test.hpp>
#include <cstring>

using namespace std;

struct SurfaceExtractorTestFixture 
{
	SurfaceExtractorTestFixture()
		: surfaceExtractor(0.0, 20.0, 0.0, 20.0, 0.0, 20.0, 1.0, 0.5, 0.01)
	{
	}

	~SurfaceExtractorTestFixture()
	{
	}

	int getLowestSlabWithTodo()
	{
		return surfaceExtractor.getLowestSlabWithTodo();
	}

	void initCache(float** particles, int particleCount, int particleSize, double rc, double cubeSize) 
	{
		surfaceExtractor._particleLookupCache.init(particles, particleCount, particleSize, rc, cubeSize);
	}

	void findSeedCubes(float** particles, int particleCount) 
	{
		surfaceExtractor.findSeedCubes(particles, particleCount);
	}

	void checkTodoInRange(int xMin, int xMax, int yMin, int yMax) 
	{
		for (vector<vector<pair<int, int> > >::iterator it = surfaceExtractor._slabs.begin(); it != surfaceExtractor._slabs.end(); ++it) {
			for (vector<pair<int, int> >::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
				BOOST_CHECK_GE(it2->first, xMin);
				BOOST_CHECK_LE(it2->second, xMax);
				BOOST_CHECK_GE(it2->first, yMin);
				BOOST_CHECK_LE(it2->second, yMax);
			}
		}
	}

	SurfaceExtractor surfaceExtractor;
};

BOOST_FIXTURE_TEST_SUITE(SurfaceExtractorTest, SurfaceExtractorTestFixture);


BOOST_AUTO_TEST_SUITE_END();