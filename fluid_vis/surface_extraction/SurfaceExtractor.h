#pragma once

#include "ParticleLookupCache.h"
#include "Polygonizer.h"
#include "Slice.h"

#include <list>
#include <vector>
#include <set>


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

	int xSize;
	int ySize;
	int zSize;

	int margin;

	Block(int capacity) 
		: _capacity(capacity), particles(0), particlesCount(0)
	{
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
};

/*
 *   
 *	 y^   ^z
 *	   \  | 
 *	    \ | 
 *	     \|         x
 *	      |--------->
 *	     
 */

class SurfaceExtractor
{
public:
	SurfaceExtractor(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, double rc, double cubeSize, double isoTreshold);
	~SurfaceExtractor();

	void extractSurface(float* particles, int particleCount, int particleComponents, int vertexComponents, float* vertices, float* normals, unsigned int* indices, int& vertCount, int& triangleCount, int maxVertices, int maxIndices);

	void extractSurface(Block& block);

	int getXSize() const { return _xSize; }
	int getYSize() const { return _ySize; }
	int getZSize() const { return _zSize; }

	double getRc() const { return _rc; }
	double getCubeSize() const { return _cubeSize; }

private:
	friend struct SurfaceExtractorTestFixture;
	friend struct SurfaceExtractorPerformanceTestFixture;

	void findSeedCubes(float** particles, int particleCount);
	
	int popLowestSlabWithTodo() 
	{
		if (_slabsWithTodo.empty())
			return -1;
		int retVal = *(_slabsWithTodo.begin());
		_slabsWithTodo.erase(retVal);
		return retVal;
	}

	void lookupOrEval(int x, int y, int z, CornerCacheEntry*& corner);

	bool cubeInMargin(int x, int y, int z) 
	{
		// TODO implement
		return false;
	}

	int getLowestSlabWithTodo() 
	{
		if (_slabsWithTodo.empty())
			return -1;
		return *(_slabsWithTodo.begin());
	}
	
	// computes normal vectors that needs to be normalized
	void computeNormals(float* vertices, unsigned int* indices, int nvert, int ntriag, float* normals);

private:
	float** _particlePtrs;

	int _maxVertices;
	int _maxIndices;
	float* _vertices;
	float* _normals;
	unsigned int* _indices;
	double _xMin;
	double _xMax;
	double _yMin;
	double _yMax;
	double _zMin;
	double _zMax;

	int _xSize;
	int _ySize;
	int _zSize;

	double _rc;
	double _cubeSize;

	double _isoTreshold;
	
	std::vector<std::vector<std::pair<int, int> > > _slabs;
	std::vector<Slice> _slices;
	std::set<int> _slabsWithTodo;

	ParticleLookupCache _particleLookupCache;

	SliceAllocatorPtr _sliceAllocator;
};

