#include "SurfaceExtractor.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

#define X 0
#define Y 1
#define Z 2

inline int round(double x)
{
	return (int)(x + 0.5);
}

SurfaceExtractor::SurfaceExtractor(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, double rc, double cubeSize, double isoTreshold)
	: _xMin(xMin), _xMax(xMax), _yMin(yMin), _yMax(yMax), _zMin(zMin), _zMax(zMax), _rc(rc), _cubeSize(cubeSize), _isoTreshold(isoTreshold),
	  _xSize(ceil((xMax - xMin) / cubeSize)), _ySize(ceil((yMax - yMin) / cubeSize)), _zSize(ceil((zMax - zMin) / cubeSize)),
	  _particleLookupCache(_xSize+1, _ySize+1, _zSize+1)
{
	_particlePtrs = new float*[64000];
	_slabs.resize(_zSize);
	_slices.resize(_zSize + 1);
	_sliceAllocator = SliceAllocator::getNewAllocator(_xSize+1, _ySize+1, 10);
}

SurfaceExtractor::~SurfaceExtractor()
{
	if (_particlePtrs) {
		delete [] _particlePtrs;
	}
}


void SurfaceExtractor::extractSurface(float* particles, int particleCount, int particleComponents, int vertexComponents, float* vertices, float* normals, unsigned int* indices, int& vertCount, int& triangleCount, int maxVertices, int maxIndices)
{
	for (int i = 0; i < particleCount; i++) {
		_particlePtrs[i] = &particles[i * particleComponents];
	}
	_particleLookupCache.init(_particlePtrs, particleCount, particleComponents, _rc, _cubeSize);

	findSeedCubes(_particlePtrs, particleCount);

	int currentIndex = 0;

	triangleCount = 0;
	vertCount = 0;

	int z;
	while ((z = popLowestSlabWithTodo()) >= 0) {
		Slice& sliceBelow = _slices[z];
		Slice& sliceAbove = _slices[z+1];

		sliceBelow.ensureAllocated(_xSize+1, _ySize+1, _sliceAllocator);
		sliceAbove.ensureAllocated(_xSize+1, _ySize+1, _sliceAllocator);
		
		vector<pair<int, int> >& slabCurrent = _slabs[z];
		vector<pair<int, int> >& slabBelow = _slabs[max(0       , z-1)];
		vector<pair<int, int> >& slabAbove = _slabs[min(_zSize-1, z+1)];

		while (!slabCurrent.empty()) {
			pair<int, int> todo = slabCurrent.back();
			slabCurrent.pop_back();
			int x = todo.first;
			int y = todo.second;

			if (sliceBelow(x, y).doneAbove || sliceAbove(x, y).doneBelow) {
				continue;
			}

			sliceBelow(x, y).doneAbove = true;
			sliceAbove(x, y).doneBelow = true;

			CornerCacheEntry* corners[8] = {0, 0, 0, 0, 0, 0, 0, 0};

			lookupOrEval(x,   y,   z,   corners[0]);
			lookupOrEval(x+1, y,   z,   corners[1]);
			lookupOrEval(x+1, y+1, z,   corners[2]);
			lookupOrEval(x,   y+1, z,   corners[3]);
			lookupOrEval(x,   y,   z+1, corners[4]);
			lookupOrEval(x+1, y,   z+1, corners[5]);
			lookupOrEval(x+1, y+1, z+1, corners[6]);
			lookupOrEval(x,   y+1, z+1, corners[7]);

			int toDo;
			if (!cubeInMargin(x, y, z)) {
				int ntriag;
				int nvert;
				_polygonizer.polygonize(corners, _isoTreshold, &vertices[vertCount*vertexComponents], vertexComponents, &indices[triangleCount*3], currentIndex, ntriag, nvert, toDo);
				currentIndex += ntriag;
				triangleCount += ntriag;
				vertCount += nvert;
			} else {
				toDo = _polygonizer.cubesToDo(corners, _isoTreshold);
			}

			// there is some inconsistency between CUBE_ABOVE, CUBE_BELOW flags and sliceBelow and sliceAbove
			if (toDo & Polygonizer::CUBE_FRONT && !sliceBelow(x, y+1).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x, y+1));
				assert(y+1 < _ySize);
			}
			if (toDo & Polygonizer::CUBE_BACK && !sliceBelow(x, y-1).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x, y-1));
				assert(y-1 >= 0);
			}
			if (toDo & Polygonizer::CUBE_RIGHT && !sliceBelow(x+1, y).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x+1, y));
				assert(x+1 < _xSize);
			}
			if (toDo & Polygonizer::CUBE_LEFT && !sliceBelow(x-1, y).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x-1, y));
				assert(x-1 >= 0);
			}
			if (toDo & Polygonizer::CUBE_ABOVE && !sliceAbove(x, y).doneAbove) {
				slabAbove.push_back(pair<int, int>(x, y));
				_slabsWithTodo.insert(z+1);
				assert(z+1 < _zSize);
			}
			if (toDo & Polygonizer::CUBE_BELOW && !sliceBelow(x, y).doneBelow) {
				slabBelow.push_back(pair<int, int>(x, y));
				_slabsWithTodo.insert(z-1);
				assert(z-1 >= 0);
			}
		}
		
		if (slabBelow.empty()) 
			sliceBelow.deallocate();
		if (slabAbove.empty())
			sliceAbove.deallocate();
	}

	computeNormals(vertices, indices, vertCount, triangleCount, normals);
}

void SurfaceExtractor::findSeedCubes(float** particles, int particleCount)
{
	for (int i = 0; i < particleCount; i++) {
		float* p = particles[i];
		int x = min(_xSize-1, (int)round((p[X] - _xMin) / _cubeSize));
		int y = min(_ySize-1, (int)round((p[Y] - _yMin) / _cubeSize));
		int z = min(_zSize-1, (int)round((p[Z] - _zMin) / _cubeSize));

		double isoVal = _isoTreshold + 0.25;

		while ((isoVal = _particleLookupCache.getFieldValueAt(x, y, z)) > _isoTreshold && (p[Z] - z * _cubeSize) < _rc)
			z--;

		if (isoVal <= _isoTreshold) {
			z = min(_xSize-1, z);
			_slabs[z].push_back(pair<int, int>(x, y));
			_slabsWithTodo.insert(z);
		}
	}
}

void SurfaceExtractor::lookupOrEval(int x, int y, int z, CornerCacheEntry*& corner)
{
	Slice& slice = _slices[z];
	SliceEntry& sliceEntry = slice(x, y);

	if (sliceEntry.cornerCacheIndex == -1) {
		sliceEntry.cornerCacheIndex = slice.cornerCache.size();
		slice.cornerCache.push_back(CornerCacheEntry(x, y, _particleLookupCache.getFieldValueAt(x, y, z), true));		
		slice.cornerCache.back().spaceCoord[X] = _xMin + x * _cubeSize;
		slice.cornerCache.back().spaceCoord[Y] = _yMin + y * _cubeSize;
		slice.cornerCache.back().spaceCoord[Z] = _zMin + z * _cubeSize;
	}

	corner = &(slice.cornerCache[sliceEntry.cornerCacheIndex]);
}



void SurfaceExtractor::computeNormals(float* vertices, unsigned int* indices, int nvert, int ntriag, float* normals)
{
	memset(normals, 0, nvert * 3 * sizeof(float));
	for (int i = 0; i < ntriag; i++) {
		float *v1 = &vertices[indices[i*3+0]];
		float *v2 = &vertices[indices[i*3+1]];
		float nx = v1[Y] * v2[Z] - v1[Z] * v2[Y];
		float ny = v1[Z] * v2[X] - v1[X] * v2[Z];
		float nz = v1[X] * v2[Y] - v1[Y] * v2[X];
		normals[indices[i*3+0]*3 + 0] += nx;
		normals[indices[i*3+0]*3 + 1] += ny;
		normals[indices[i*3+0]*3 + 2] += nz;

		normals[indices[i*3+1]*3 + 0] += nx;
		normals[indices[i*3+1]*3 + 1] += ny;
		normals[indices[i*3+1]*3 + 2] += nz;

		normals[indices[i*3+2]*3 + 0] += nx;
		normals[indices[i*3+2]*3 + 1] += ny;
		normals[indices[i*3+2]*3 + 2] += nz;
	}
}