#define DEBUG

#include "TestUtils.h"
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
		memset(data, 0, MAX_DATA_SIZE*sizeof(float));
	}

	~FiltersTestFixture()
	{
		if (data)
			delete [] data;
	}

	float* data;
	static const int MAX_DATA_SIZE = 1024;
};

BOOST_FIXTURE_TEST_SUITE(FiltersTest, FiltersTestFixture);

BOOST_AUTO_TEST_CASE(create1DSize11Scale1)
{
	const int TEST_SIZE = 11;

	Filters::createGauss1D(TEST_SIZE, 1.0, 0.1, data);
	for (int i = 0; i < TEST_SIZE/2; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], data[TEST_SIZE - 1 - i], TestUtils::FLOAT_PRECISION);
	}
}

BOOST_AUTO_TEST_CASE(create1DSize11Scale05)
{
	const int TEST_SIZE = 11;

	Filters::createGauss1D(TEST_SIZE, 0.5, 0.1, data);
	for (int i = 0; i < TEST_SIZE/2; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], data[TEST_SIZE - 1 - i], TestUtils::FLOAT_PRECISION);
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
			BOOST_CHECK_CLOSE_FRACTION(data[i*TEST_SIZE + j], data[i*TEST_SIZE + TEST_SIZE - 1 - j], TestUtils::FLOAT_PRECISION);
			// check columns
			BOOST_CHECK_CLOSE_FRACTION(data[j*TEST_SIZE + i], data[(TEST_SIZE - 1 - j) * TEST_SIZE + i], TestUtils::FLOAT_PRECISION);
		}
	}
}

BOOST_AUTO_TEST_CASE(normalize1D)
{
	const int TEST_SIZE = 11;
	Filters::createGauss1D(TEST_SIZE, 0.5, 0.5, data);
	Filters::normalize(data, 1, TEST_SIZE);

	double sum = 0.0;
	for (int i = 0; i < TEST_SIZE; i++) {
		sum += data[i];
	}

	BOOST_CHECK_CLOSE_FRACTION(sum, 1, TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(createHeaviside)
{
	const int TEST_SIZE = 11;

	Filters::createHeavisideDistribution(0.0, 1.0, 0.5, TEST_SIZE, data);

	for (int i = 0; i < TEST_SIZE / 2; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], 1.0f, TestUtils::FLOAT_PRECISION);
	}
	for (int i = TEST_SIZE / 2; i < TEST_SIZE; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[i], 0.0f, TestUtils::FLOAT_PRECISION);
	}
}

BOOST_AUTO_TEST_CASE(createGauss1DArray_properRetVal)
{
	const int MAX_SIZE = 21;
	int retVal = Filters::createGauss1DArray(MAX_SIZE, 0.2, data);
	BOOST_CHECK_EQUAL(retVal, (MAX_SIZE - 3) / 2 + 1);
}

BOOST_AUTO_TEST_CASE(createGauss1DArray_properValuesAtEdgesAndInTheMiddle)
{
	const int MAX_SIZE = 21;
	int size = Filters::createGauss1DArray(MAX_SIZE, 0.2, data);
	int s = MAX_SIZE;
	for (int i = size; i < size; i++) {
		BOOST_CHECK_CLOSE_FRACTION(data[MAX_SIZE*i], 0.2, TestUtils::FLOAT_PRECISION);
		BOOST_CHECK_CLOSE_FRACTION(data[MAX_SIZE*i + s - 1], 0.2, TestUtils::FLOAT_PRECISION);
		BOOST_CHECK_CLOSE_FRACTION(data[MAX_SIZE*i + s/2 -1], 1.0, TestUtils::FLOAT_PRECISION);
		s -= 2;
	}
}

BOOST_AUTO_TEST_CASE(createGauss1DArrayAsc_properRetVal)
{
	const int MAX_WIDTH = 21;
	int number_of_filters_returned = Filters::createGauss1DArrayAsc(MAX_WIDTH, 0.2, data);
	int number_of_filters_required = (MAX_WIDTH - 1) / 2 + 1;
	BOOST_CHECK_EQUAL(number_of_filters_returned, number_of_filters_required);
}

BOOST_AUTO_TEST_CASE(createGauss1DArrayAsc_properValuesAtEdges)
{
	const int MAX_WIDTH = 21;
	const double VALUE_AT_EDGE = 0.2;
	int numOfFilters = Filters::createGauss1DArrayAsc(MAX_WIDTH, VALUE_AT_EDGE, data);
	for (int i = 1; i < numOfFilters; i++) {
		float* currentData = &data[i * MAX_WIDTH];
		double VAL_AT_LEFT_EDGE = currentData[0];
		double VAL_AT_RIGHT_EDGE = currentData[2*i];
		BOOST_CHECK_CLOSE_FRACTION(VAL_AT_LEFT_EDGE, VALUE_AT_EDGE, TestUtils::FLOAT_PRECISION);
		BOOST_CHECK_CLOSE_FRACTION(VAL_AT_RIGHT_EDGE, VALUE_AT_EDGE, TestUtils::FLOAT_PRECISION);
	}
}

BOOST_AUTO_TEST_CASE(createGauss1DArrayAsc_zeroBehindEdges)
{
	const int MAX_WIDTH = 21;
	const double VALUE_AT_EDGE = 0.2;
	int numOfFilters = Filters::createGauss1DArrayAsc(MAX_WIDTH, VALUE_AT_EDGE, data);
	for (int i = 1; i < numOfFilters-1; i++) {
		float* currentData = &data[i * MAX_WIDTH];
		double VAL_BEHIND_RIGHT_EDGE = currentData[2*i + 1];
		BOOST_CHECK_CLOSE_FRACTION(VAL_BEHIND_RIGHT_EDGE, 0.0, TestUtils::FLOAT_PRECISION);
	}
}

BOOST_AUTO_TEST_CASE(createGauss1DArrayAsc_propertValuesInTheMiddle)
{
	const int MAX_WIDTH = 21;
	const double VALUE_AT_EDGE = 0.2;
	int numOfFilters = Filters::createGauss1DArrayAsc(MAX_WIDTH, VALUE_AT_EDGE, data);
	for (int i = 0; i < numOfFilters; i++) {
		float* currentData = &data[i * MAX_WIDTH];
		double VAL_IN_THE_MIDDLE = currentData[i];
		BOOST_CHECK_CLOSE_FRACTION(VAL_IN_THE_MIDDLE, 1.0, TestUtils::FLOAT_PRECISION);
	}

	TestUtils::printArray(data, MAX_WIDTH, numOfFilters);
}

BOOST_AUTO_TEST_SUITE_END();