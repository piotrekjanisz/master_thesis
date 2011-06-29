#pragma once

#include <list>

class ParticleLookupCache 
{
	struct ParticleLookupCacheEntry
	{
		int minZ;
		int midZ;
		int maxZ;
		float* particle;

		ParticleLookupCacheEntry(int _minZ, int _maxZ, int _midZ, float* _particle)
			: minZ(_minZ), maxZ(_maxZ), midZ(_midZ), particle(_particle) {}
	};

	std::list<ParticleLookupCacheEntry>** _data;
	int _sizeX;
	int _sizeY;
	int _sizeZ;
	double _rc;
	double _cubeSize;

public:
	ParticleLookupCache(int sizeX, int sizeY, int sizeZ);
	~ParticleLookupCache();

	void init(float** particles, int particleCount, int particleSize, double rc, double cubeSize);

	double getFieldValueAt(int x, int y, int z);
};


