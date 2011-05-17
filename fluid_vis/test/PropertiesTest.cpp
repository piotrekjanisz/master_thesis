#include <fluid_vis/Properties.h>
#include <boost/test/unit_test.hpp>
#include <vmmlib\vmmlib.hpp>

using namespace std;

struct PropertiesTestFixture
{
	PropertiesTestFixture()
		: mTestFile("test_config/test1.cfg"), mWrongTestFile("some\file\foo")
	{
		mProperties.load(mTestFile);
	}

	~PropertiesTestFixture()
	{
	}

	Properties mProperties;
	string mTestFile;
	string mWrongTestFile;
};

BOOST_FIXTURE_TEST_SUITE(PropertiesTest, PropertiesTestFixture);

BOOST_AUTO_TEST_CASE(LoadTestConfigFile)
{
	BOOST_CHECK_NO_THROW(mProperties.load(mTestFile));
	BOOST_CHECK_THROW(mProperties.load(mWrongTestFile), exception);
}

BOOST_AUTO_TEST_CASE(HasKeyOfType)
{
	BOOST_CHECK(mProperties.hasKeyOfType<int>("int1"));
	BOOST_CHECK(mProperties.hasKeyOfType<int>("int2"));
	BOOST_CHECK(mProperties.hasKeyOfType<float>("float1"));
	BOOST_CHECK(mProperties.hasKeyOfType<float>("float2"));
	BOOST_CHECK(mProperties.hasKeyOfType<vmml::vec3f>("vec1"));
	BOOST_CHECK(mProperties.hasKeyOfType<string>("str1"));
}

BOOST_AUTO_TEST_CASE(GetProperties)
{
	BOOST_CHECK_EQUAL(mProperties.get<int>("int1"), 123);
	BOOST_CHECK_EQUAL(mProperties.get<int>("int2"), -123);
	BOOST_CHECK_EQUAL(mProperties.get<float>("float1"), 123.0f);
	BOOST_CHECK_EQUAL(mProperties.get<float>("float2"), -123.123f);
	BOOST_CHECK_EQUAL(mProperties.get<vmml::vec3f>("vec1"), vmml::vec3f(1.0f, 1.0f, -1.0f));
	BOOST_CHECK_EQUAL(mProperties.get<string>("str1"), string("Ala ma kota"));
}

BOOST_AUTO_TEST_SUITE_END();