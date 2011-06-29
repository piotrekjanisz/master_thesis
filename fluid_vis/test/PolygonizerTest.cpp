#define DEBUG


#include "TestUtils.h"
#include <boost/test/unit_test.hpp>
#include <surface_extraction\ParticleLookupCache.h>
#include <surface_extraction\Polygonizer.h>
#include <fluid_vis\debug_utils.h>


struct PolygonizerTestFixture 
{
	PolygonizerTestFixture()
	{
	}

	~PolygonizerTestFixture()
	{
	}

	Polygonizer polygonizer;
};

BOOST_FIXTURE_TEST_SUITE(PolygonizerTest, PolygonizerTestFixture);

BOOST_AUTO_TEST_SUITE_END();