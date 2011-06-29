#include "TestUtils.h"

#include <boost/test/unit_test.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <cstring>
#include <iostream>
#include <list>
#include <vector>
#include <utility>

struct AllocatorPerformanceTestFixture
{
	AllocatorPerformanceTestFixture() {}
	~AllocatorPerformanceTestFixture() {}
};

struct DummyStruct
{
	int x;
	int y;
	double fieldValue;
	bool cubeDone;
	int lx;
	int ly;
	int lz;
};

BOOST_FIXTURE_TEST_SUITE(AllocatorPerformanceTest, AllocatorPerformanceTestFixture);

BOOST_AUTO_TEST_CASE(listInsertTest_standardAllocator)
{
	typedef std::list<std::pair<int, int> > TestList;
	TestList l;
	const int INSERTION_COUNT = 1000000;

	MEASURE_TIME("listInsertTest_standardAllocator",
		for (int i = 0; i < INSERTION_COUNT; i++) {
			l.push_back(std::pair<int, int>(1, 1));
		}
		l.clear();
	)
}

BOOST_AUTO_TEST_CASE(listInsertTest_boostFastPoolAllocator)
{
	typedef std::list<std::pair<int, int>, boost::fast_pool_allocator<DummyStruct> > TestList;
	TestList l;
	const int INSERTION_COUNT = 1000000;

	MEASURE_TIME("listInsertTest_boostFastPoolAllocator",
		for (int i = 0; i < INSERTION_COUNT; i++) {
			l.push_back(std::pair<int, int>(1, 1));
		}
		l.clear();
	)
}

BOOST_AUTO_TEST_CASE(vectorInsertTest)
{
	typedef std::vector<std::pair<int, int> > TestList;
	TestList l;
	const int INSERTION_COUNT = 1000000;

	MEASURE_TIME("vectorInsertTest",
		for (int i = 0; i < INSERTION_COUNT; i++) {
			l.push_back(std::pair<int, int>(1, 1));
		}
		l.clear();
	)
}

BOOST_AUTO_TEST_CASE(arrayInsertTest)
{
	const int INSERTION_COUNT = 1000000;
	DummyStruct* l;
	MEASURE_TIME("arrayInsertTest",
		l = new DummyStruct[INSERTION_COUNT];
		delete [] l;
	)
}

BOOST_AUTO_TEST_SUITE_END();