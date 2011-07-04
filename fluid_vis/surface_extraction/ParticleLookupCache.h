#pragma once

#include <list>
#include <vector>
#include <boost/pool/pool_alloc.hpp>

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

	typedef std::vector<ParticleLookupCacheEntry> ListType;
	std::pair<int, ListType>* _data;
	
	std::pair<int, ListType>& get(int x, int y)
	{
		return _data[x * _sizeY + y];
	}
	
	int _sizeX;
	int _sizeY;
	int _sizeZ;
	double _xMin;
	double _xMax;
	double _yMin;
	double _yMax;
	double _zMin;
	double _zMax;
	double _rc;
	double _cubeSize;

public:
	ParticleLookupCache(int sizeX, int sizeY, int sizeZ);
	ParticleLookupCache(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, double rc, double cubeSize);
	~ParticleLookupCache();

	void init(float** particles, int particleCount, int particleSize, double rc, double cubeSize);
	void init(float** particles, int particleCount, int particleSize);

	double getFieldValueAt(int x, int y, int z);

	double getFieldValueAt2(int x, int y, int z);
};


