#pragma once

#include <cmath>
#include <vector>

class Block
{
	int _capacity;
public:
	float** particles;
	int particlesCount;

	double xMin;
	double xMax;
	double yMin;
	double yMax;
	double zMin;
	double zMax;

	double rc;
	double cubeSize;

	int xSize;
	int ySize;
	int zSize;

	int margin;

	Block(int capacity, double _xMin, double _xMax, double _yMin, double _yMax, double _zMin, double _zMax, double _rc, double _cubeSize) 
		: _capacity(capacity), particles(0), particlesCount(0), xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax), zMin(_zMin), zMax(_zMax), rc(_rc), cubeSize(_cubeSize)
	{
		xSize = ceil((xMax - xMin) / cubeSize);
		ySize = ceil((yMax - yMin) / cubeSize);
		zSize = ceil((zMax - zMin) / cubeSize);
		margin = ceil(rc / cubeSize);
		particles = new float*[_capacity];
	}

	~Block()
	{
		if (particles)
			delete [] particles;
	}

	void addParticles(float* particle)
	{
		particles[particlesCount++] = particle;
	}

	void clearParticles()
	{
		particlesCount = 0;
	}

	bool cubeInMargin(int x, int y, int z)
	{
		return (x < margin || xSize - x < margin) ||
			(y < margin || ySize - y < margin) ||
			(z < margin || zSize - z < margin);
	}
};