#pragma once

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
		std::cout << _posBuffer[i*componentNum + j];
		for (int j = 1; j < componentNum; j++)
			std::cout << ", " << _posBuffer[i*componentNum + j];
		std::cout << std::endl;
	}
}