#pragma once

class DebugData
{
	float* _densityBuffer;
	unsigned _densityBufferSize;
	float _minDensity;
	float _maxDensity;

public:
	DebugData();
	~DebugData();

	void setDensityBuffer(float* densityBuffer, unsigned densityBufferSize);

	void printDensityRange();
};