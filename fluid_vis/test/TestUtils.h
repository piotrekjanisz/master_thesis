#pragma once

#include <iostream>
#include <boost/timer.hpp>
#include <iostream>

#define MEASURE_TIME(title, code) \
	{ \
		boost::timer clock; \
		{ code } \
		std::cout << title << std::endl << "\tELAPSED: " << clock.elapsed() << "s" << std::endl; \
	}

class TestUtils
{
public:
	static const float FLOAT_PRECISION;

	template <typename T> 
	static void printArray(T* a, int components, int count);

	static void generateVectors(float* v, int count, int components, float min, float max, int seed);
};

template <typename T>
void TestUtils::printArray(T* a, int components, int count)
{
	for (int i = 0; i < count; i++) {
		std::cout << a[i * components];
		for (int j = 1; j < components; j++) {
			std::cout << ", " << a[i*components + j];
		}
		std::cout << std::endl;
	}
}

template <typename T>
class MemHandle
{
private:
	T* _mem;
	MemHandle& operator=(const MemHandle& other) {}
	MemHandle(const MemHandle& other) {}

public:
	MemHandle(T* mem)
		: _mem(mem) {}
	~MemHandle()
	{
		if (_mem) delete _mem;
	}
};

template <typename T>
class ArrayHandle
{
private:
	T* _mem;
	ArrayHandle& operator=(const ArrayHandle& other) {}
	ArrayHandle(const ArrayHandle& other) {}
public:
	ArrayHandle(T* mem)
		: _mem(mem) {}
	~ArrayHandle() { if (_mem) delete [] _mem; }
};

