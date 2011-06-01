#pragma once

#ifdef DEBUG
#define DEBUG_CODE(code) (code)
#define DEBUG_COUT(code) (std::cout code)

#include <iostream>

namespace DebugUtils
{
	template <typename T> 
	void printArray(T* array, int size, int componentNum);
};

template <typename T> 
void DebugUtils::printArray(T* array, int size, int componentNum)
{
	for (int i = 0; i < size; i++) {
		std::cout << array[i*componentNum + 0];
		for (int j = 1; j < componentNum; j++)
			std::cout << ", " << array[i*componentNum + j];
		std::cout << std::endl;
	}
}
#else
#define DEBUG_CODE(code)
#define DEBUG_COUT(code)
#endif