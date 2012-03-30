#include "DebugData.h"
#include "debug_utils.h"

#include <iostream>
#include <algorithm>

using namespace std;


DebugData::DebugData()
	: _densityBuffer(NULL)
{
}

DebugData::~DebugData()
{
}

void DebugData::setDensityBuffer(float* densityBuffer, unsigned densityBufferSize) 
{ 
	_densityBuffer = densityBuffer; 
	_densityBufferSize = densityBufferSize;

	_minDensity = *min_element(_densityBuffer, _densityBuffer + _densityBufferSize);
	_maxDensity = *max_element(_densityBuffer, _densityBuffer + _densityBufferSize);
}

void DebugData::printDensityRange()
{
	DEBUG_PRINT_VAR(_minDensity);
	DEBUG_PRINT_VAR(_maxDensity);
}