#define DEBUG

#include <fluid_vis/filters.h>
#include <boost/test/unit_test.hpp>
#include <fluid_vis\debug_utils.h>

using namespace std;

struct FiltersTestFixture
{
	FiltersTestFixture()
		: data(NULL)
	{
		data = new float[MAX_DATA_SIZE];
	}

	~FiltersTestFixture()
	{
		if (data)
			delete [] data;
	}

	static const float FLOAT_PRECISION;

	float* data;
	static const int MAX_DATA_SIZE = 1024;
};

const float FiltersTestFixture::FLOAT_PRECISION = 0.00000000001f;

BOOST_FIXTURE_TEST_SUITE(FiltersTest, FiltersTestFixture);

BOOST_AUTO_TEST_CASE(create1DSize11Scale1)
{
	const int TEST_SIZE = 11;

	Filters::createGauss1D(TEST_SIZE, 1.0, 0.1, data);
	for (int i = 0; i < TEST_SIZE/2; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], data[TEST_SIZE - 1 - i], FLOAT_PRECISION);
	}
}

BOOST_AUTO_TEST_CASE(create1DSize11Scale05)
{
	const int TEST_SIZE = 11;

	Filters::createGauss1D(TEST_SIZE, 0.5, 0.1, data);
	for (int i = 0; i < TEST_SIZE/2; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], data[TEST_SIZE - 1 - i], FLOAT_PRECISION);
	}
}

#define GET_ROW_MAJOR(tab, row, col, size) (tab)[(row)*(size) + col]
#define GET_COL_MAJOR(tab, col, row, size) (tab)[

BOOST_AUTO_TEST_CASE(create2DSize11Scale05)
{
	const int TEST_SIZE = 11;

	Filters::createGauss2D(TEST_SIZE, 0.5, 0.1, data);

	for (int i = 0; i < TEST_SIZE; i++) {
		for (int j = 0; j < TEST_SIZE/2; j++) {
			// check rows
			BOOST_CHECK_CLOSE_FRACTION(data[i*TEST_SIZE + j], data[i*TEST_SIZE + TEST_SIZE - 1 - j], FLOAT_PRECISION);
			// check columns
			BOOST_CHECK_CLOSE_FRACTION(data[j*TEST_SIZE + i], data[(TEST_SIZE - 1 - j) * TEST_SIZE + i], FLOAT_PRECISION);
		}
	}
}

BOOST_AUTO_TEST_CASE(createHeaviside)
{
	const int TEST_SIZE = 11;

	Filters::createHeavisideDistribution(0.0, 1.0, 0.5, TEST_SIZE, data);

	for (int i = 0; i < TEST_SIZE / 2; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], 1.0f, FLOAT_PRECISION);
	}
	for (int i = TEST_SIZE / 2; i < TEST_SIZE; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], 0.0f, FLOAT_PRECISION);
	}
}

BOOST_AUTO_TEST_SUITE_END();