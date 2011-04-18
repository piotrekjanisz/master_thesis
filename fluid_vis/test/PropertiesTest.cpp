//#pragma comment(lib, "boost_unit_test_framework-vc100-mt-gd")
#define BOOST_TEST_MODULE PropertiesTest
#include <boost\test\unit_test.hpp>

BOOST_AUTO_TEST_CASE(get_test)
{
	Properties properties;
	BOOST_CHECK_THROW(properties.get<int>("ala"), ElementNotFoundException);
}

int main(int argc, char** argv)
{
	return 1;
}