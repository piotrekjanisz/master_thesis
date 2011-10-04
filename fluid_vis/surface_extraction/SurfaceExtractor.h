#pragma once

#include "ParticleLookupCache.h"
#include "Polygonizer.h"
#include "Slice.h"
#include "Block.h"
#include "data_types.h"

#include <list>
#include <vector>
#include <set>


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
	SurfaceExtractor(const SurfaceExtractorDesc& desc);
	~SurfaceExtractor();

	void extractSurface(Block& block, int vertexComponents, float* vertices, float* normals, unsigned int* indices, int& nvert, int& ntriag, int maxVert, int maxTriag);

private:	// functions
	friend struct SurfaceExtractorTestFixture;
	friend struct SurfaceExtractorPerformanceTestFixture;

	void findSeedCubes(Block& block, float** particles, int particleCount);
	
	int popLowestSlabWithTodo() 
	{
		if (_slabsWithTodo.empty())
			return -1;
		int retVal = *(_slabsWithTodo.begin());
		_slabsWithTodo.erase(retVal);
		return retVal;
	}

	void lookupOrEval(Block& block, int x, int y, int z, CornerCacheEntry*& corner);

	int getLowestSlabWithTodo() 
	{
		if (_slabsWithTodo.empty())
			return -1;
		return *(_slabsWithTodo.begin());
	}
	
	// computes normal vectors that needs to be normalized
	void computeNormals(float* vertices, unsigned int* indices, int nvert, int ntriag, float* normals);

	void computeNormalsAngleWeight(float* vertices, unsigned int* indices, int nvert, int ntriag, float* normals);

private: // fields
	int _xSize;
	int _ySize;
	int _zSize;

	double _isoTreshold;
	
	std::vector<std::vector<std::pair<int, int> > > _slabs;
	std::vector<Slice> _slices;
	std::set<int> _slabsWithTodo;

	ParticleLookupCache _particleLookupCache;

	SliceAllocatorPtr _sliceAllocator;
};

