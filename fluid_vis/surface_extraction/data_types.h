#pragma once

#include <cmath>

struct SurfaceExtractorDesc
{
	double xMin;
	double xMax;
	double yMin;
	double yMax;
	double zMin;
	double zMax;
	
	double rc;
	double isoTreshold;

	double blockSize;
	double cubeSize;

	int maxParticles;

	int maxTrianglesPerThread;
	int maxVerticesPerThread;

	int threads;

	int getXBlocks() const
	{
		return ceil((xMax - xMin) / blockSize);
	}

	int getYBlocks() const
	{
		return ceil((yMax - yMin) / blockSize);
	}

	int getZBlocks() const
	{
		return ceil((zMax - zMin) / blockSize);
	}

	int getCubesInBlock() const
	{
		return ceil((blockSize + 2*rc) / cubeSize);
	}

	int getMargin() const 
	{
		return ceil(rc / cubeSize);
	}
};

struct TriangleMesh
{
	float* vertices;
	float* normals;
	unsigned int* indices;
	
	int verticesCount;
	int trianglesCount;
};
