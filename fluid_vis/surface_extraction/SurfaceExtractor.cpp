#include "SurfaceExtractor.h"
#include <utils\utils.h>
#include <utils\mymath.h>
#include <iostream>
#include <cassert>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;
using namespace MyMath;

#define X 0
#define Y 1
#define Z 2

inline int myRound(double x)
{
	return ceil(x - 0.5);
}

SurfaceExtractor::SurfaceExtractor(const SurfaceExtractorDesc& desc)
	: _xSize(desc.getCubesInBlock()), _ySize(desc.getCubesInBlock()), _zSize(desc.getCubesInBlock()), 
	_particleLookupCache(desc.getCubesInBlock()+1, desc.getCubesInBlock()+1, desc.getCubesInBlock()+1),
	_isoTreshold(desc.isoTreshold)
{
	_slabs.resize(_zSize);
	_slices.resize(_zSize + 1);
	_sliceAllocator = SliceAllocator::getNewAllocator(_xSize+1, _ySize+1, 10);
}

SurfaceExtractor::~SurfaceExtractor()
{
}

void SurfaceExtractor::extractSurface(Block& block, int vertexComponents, float* vertices, float* normals, unsigned int* indices, int& nvert, int& ntriag, int maxVert, int maxTriag)
{
	//Polygonizer polygonizer(_isoTreshold, vertices, vertexComponents, indices);
	Polygonizer polygonizer(_isoTreshold, vertices, vertexComponents, normals, 3, indices);

	_particleLookupCache.init(block);

	findSeedCubes(block, block.particles, block.particlesCount);

	int currentIndex = 0;

	ntriag = 0;
	nvert = 0;

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

			lookupOrEval(block, x,   y,   z,   corners[0]);
			lookupOrEval(block, x+1, y,   z,   corners[1]);
			lookupOrEval(block, x+1, y+1, z,   corners[2]);
			lookupOrEval(block, x,   y+1, z,   corners[3]);
			lookupOrEval(block, x,   y,   z+1, corners[4]);
			lookupOrEval(block, x+1, y,   z+1, corners[5]);
			lookupOrEval(block, x+1, y+1, z+1, corners[6]);
			lookupOrEval(block, x,   y+1, z+1, corners[7]);

			int toDo;
			if (!block.cubeInMargin(x, y, z)) {
                polygonizer.polygonize(corners, toDo);
			} else {
				toDo = polygonizer.cubesToDo(corners);
			}

			// there is some inconsistency between CUBE_ABOVE, CUBE_BELOW flags and sliceBelow and sliceAbove
			if (toDo & Polygonizer::CUBE_FRONT && !sliceBelow(x, y+1).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x, y+1));
			}
			if (toDo & Polygonizer::CUBE_BACK && !sliceBelow(x, y-1).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x, y-1));
			}
			if (toDo & Polygonizer::CUBE_RIGHT && !sliceBelow(x+1, y).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x+1, y));
			}
			if (toDo & Polygonizer::CUBE_LEFT && !sliceBelow(x-1, y).doneAbove) {
				slabCurrent.push_back(pair<int, int>(x-1, y));
			}
			if (toDo & Polygonizer::CUBE_ABOVE && !sliceAbove(x, y).doneAbove) {
				slabAbove.push_back(pair<int, int>(x, y));
				_slabsWithTodo.insert(z+1);
			}
			if (toDo & Polygonizer::CUBE_BELOW && !sliceBelow(x, y).doneBelow) {
				slabBelow.push_back(pair<int, int>(x, y));
				_slabsWithTodo.insert(z-1);
			}
		}
		
		if (slabBelow.empty()) 
			sliceBelow.deallocate();
		if (slabAbove.empty())
			sliceAbove.deallocate();
	}

    nvert = polygonizer.getVerticesNumber();
    ntriag = polygonizer.getTriangleNumber();

	//computeNormals(vertices, indices, nvert, ntriag, normals);
	//computeNormalsAngleWeight(vertices, indices, nvert, ntriag, normals);
}

void SurfaceExtractor::findSeedCubes(Block& block, float** particles, int particleCount)
{
	for (int i = 0; i < particleCount; i++) {
		float* p = particles[i];
		int x = myRound((p[X] - block.xMin) / block.cubeSize);
		int y = myRound((p[Y] - block.yMin) / block.cubeSize);
		int z = max(0, min(_zSize-1, myRound((p[Z] - block.zMin) / block.cubeSize)));

		if (x >= _xSize || x < 0 || y >= _ySize || y < 0)
			continue;

		double isoVal = _isoTreshold + 0.25;

		while ((isoVal = _particleLookupCache.getFieldValueAt2(x, y, z)) > _isoTreshold && (p[Z] - z * block.cubeSize) < block.rc)
			z--;

		if (isoVal <= _isoTreshold) {
			z = max(0, z);
			_slabs[z].push_back(pair<int, int>(x, y));
			_slabsWithTodo.insert(z);
		}
	}
}

void SurfaceExtractor::lookupOrEval(Block& block, int x, int y, int z, CornerCacheEntry*& corner)
{
	Slice& slice = _slices[z];
	SliceEntry& sliceEntry = slice(x, y);

	if (sliceEntry.cornerCacheIndex == -1) {
		sliceEntry.cornerCacheIndex = slice.cornerCache.size();
		CornerCacheEntry entry(x, y, 0, true);
		entry.fieldValue = _particleLookupCache.getFieldValueAndNormalAt(x, y, z, entry.normal);
		//slice.cornerCache.push_back(CornerCacheEntry(x, y, _particleLookupCache.getFieldValueAt2(x, y, z), true));		
		slice.cornerCache.push_back(entry);
		slice.cornerCache.back().spaceCoord[X] = block.xMin + x * block.cubeSize;
		slice.cornerCache.back().spaceCoord[Y] = block.yMin + y * block.cubeSize;
		slice.cornerCache.back().spaceCoord[Z] = block.zMin + z * block.cubeSize;
	}

	corner = &(slice.cornerCache[sliceEntry.cornerCacheIndex]);
}

void SurfaceExtractor::computeNormals(float* vertices, unsigned int* indices, int nvert, int ntriag, float* normals)
{
	memset(normals, 0, nvert * 3 * sizeof(float));
	for (int i = 0; i < ntriag; i++) {
		unsigned int i1 = indices[i*3 + 0];
		unsigned int i2 = indices[i*3 + 1];
		unsigned int i3 = indices[i*3 + 2];
		
		float *v1 = &vertices[i1 * 4];
		float *v2 = &vertices[i2 * 4];
		float *v3 = &vertices[i3 * 4];

		float x1 = v2[X] - v1[X];
		float y1 = v2[Y] - v1[Y];
		float z1 = v2[Z] - v1[Z];

		float x2 = v3[X] - v1[X];
		float y2 = v3[Y] - v1[Y];
		float z2 = v3[Z] - v1[Z];

		float nx = y1 * z2 - z1 * y2;
		float ny = z1 * x2 - x1 * z2;
		float nz = x1 * y2 - y1 * x2;
		
		//normalize(nx, ny, nz);
		
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


void SurfaceExtractor::computeNormalsAngleWeight(float* vertices, unsigned int* indices, int nvert, int ntriag, float* normals)
{
	memset(normals, 0, nvert * 3 * sizeof(float));

	for (int i = 0; i < ntriag; i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int i1 = indices[i*3 + (j + 0) % 3];
			unsigned int i2 = indices[i*3 + (j + 1) % 3];
			unsigned int i3 = indices[i*3 + (j + 2) % 3];
			float *v1 = &vertices[i1 * 4];
			float *v2 = &vertices[i2 * 4];
			float *v3 = &vertices[i3 * 4];
			
			float vec1[3];
			float vec2[3];

			vec_subs(v1, v2, vec1);
			vec_subs(v3, v2, vec2);

			float cross[3];
			vec_cross(vec2, vec1, cross);
			/*
			float vec1_len = vec_len(vec1);
			float vec2_len = vec_len(vec2);
			float cross_len = vec_len(cross);

			float sin_alfa = cross_len / (vec1_len * vec2_len);
			
			float alfa = asin(sin_alfa);
			*/
			float alfa = vec_angle(vec2, vec1);
			//float alfa = 1.0f;

			normals[i2*3 + 0] += cross[0] * alfa;
			normals[i2*3 + 1] += cross[1] * alfa;
			normals[i2*3 + 2] += cross[2] * alfa;
		}
	}
}
