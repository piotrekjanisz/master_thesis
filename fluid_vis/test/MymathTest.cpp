#define DEBUG

#include "TestUtils.h"
#include "utils/mymath.h"
#include <boost/test/unit_test.hpp>

using namespace std;

struct MymathTestFixture
{
	MymathTestFixture()
	{
	}

	~MymathTestFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(MymathTest, MymathTestFixture);

const float SQRT2 = 1.4142135623730950488016887242097f;

BOOST_AUTO_TEST_CASE(crossProduct)
{
	float vec1[3] = {1.0f, 0.0f, 0.0f};
	float vec2[3] = {0.0f, 1.0f, 0.0f};
	float cross[3];

	MyMath::vec_cross(vec1, vec2, cross);

	BOOST_CHECK_CLOSE_FRACTION(cross[0], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[1], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[2], 1.0f, TestUtils::FLOAT_PRECISION);

	MyMath::vec_cross(vec2, vec1, cross);

	BOOST_CHECK_CLOSE_FRACTION(cross[0], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[1], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[2], -1.0f, TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(vectorLength)
{
	float vec1[3] = {1.0f, 0.0f, 0.0f};
	float vec2[3] = {3.0f, 4.0f, 0.0f};
	float vec3[3] = {-4.0f, 0.0f, 3.0f};

	BOOST_CHECK_CLOSE_FRACTION(MyMath::vec_len(vec1), 1.0, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(MyMath::vec_len(vec2), 5.0, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(MyMath::vec_len(vec3), 5.0, TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(vectorLength2)
{
	float vec1[3] = {SQRT2, SQRT2, 0.0f};
	float vec2[3] = {-SQRT2, 0.0f, SQRT2};
	float vec3[3] = {-SQRT2, 0.0f, -SQRT2};

	BOOST_CHECK_CLOSE_FRACTION(MyMath::vec_len(vec1), 2.0, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(MyMath::vec_len(vec2), 2.0, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(MyMath::vec_len(vec3), 2.0, TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(vectorSubs)
{
	float vec1[3] = {3.0f, 3.0f, 3.0f};
	float vec2[3] = {1.0f, 0.0f, 1.0f};
	float result[3];

	MyMath::vec_subs(vec1, vec2, result);
	BOOST_CHECK_CLOSE_FRACTION(result[0], 2.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(result[1], 3.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(result[2], 2.0f, TestUtils::FLOAT_PRECISION);

	MyMath::vec_subs(vec2, vec1, result);
	BOOST_CHECK_CLOSE_FRACTION(result[0], -2.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(result[1], -3.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(result[2], -2.0f, TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_CASE(crossProduct2)
{
	
	float vec1[3] = {SQRT2, SQRT2, 0.0f};
	float vec2[3] = {SQRT2, -SQRT2, 0.0f};
	float cross[3];

	MyMath::vec_cross(vec1, vec2, cross);
	BOOST_CHECK_CLOSE_FRACTION(cross[0], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[1], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[2], -4.0f, TestUtils::FLOAT_PRECISION);

	MyMath::vec_cross(vec2, vec1, cross);
	BOOST_CHECK_CLOSE_FRACTION(cross[0], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[1], 0.0f, TestUtils::FLOAT_PRECISION);
	BOOST_CHECK_CLOSE_FRACTION(cross[2], 4.0f, TestUtils::FLOAT_PRECISION);
}

BOOST_AUTO_TEST_SUITE_END();