#include "ParticleLookupCache.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>

using namespace std;

#define X 0
#define Y 1
#define Z 2

ParticleLookupCache::ParticleLookupCache(int sizeX, int sizeY, int sizeZ)
	: _sizeX(sizeX), _sizeY(sizeY), _sizeZ(sizeZ)
{
	_data = new std::pair<int, ListType>[sizeX * sizeY];
}

ParticleLookupCache::ParticleLookupCache(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, double rc, double cubeSize)
	: _xMin(xMin), _xMax(xMax), _yMin(yMin), _yMax(yMax), _zMin(zMin), _zMax(zMax),
	  _sizeX(ceil((xMax - xMin) / cubeSize) + 1), _sizeY(ceil((yMax - yMin) / cubeSize) + 1), _sizeZ(ceil((zMax - zMin) / cubeSize) + 1), _rc(rc), _cubeSize(cubeSize)
{
	_data = new std::pair<int, ListType>[_sizeX * _sizeY];
}

ParticleLookupCache::~ParticleLookupCache()
{
	if (_data) {
		delete [] _data;
	}
}

struct ParticleComp
{
	ParticleComp(int component) : _component(component) {}
	bool operator()(float* p1, float* p2) 
	{
		return p1[_component] < p2[_component];
	}

private:
	int _component;
};


void ParticleLookupCache::init(float** particles, int particleCount, int particleSize, double rc, double cubeSize)
{
	_rc = rc;
	_cubeSize = cubeSize;

	sort(particles, particles + particleCount, ParticleComp(2));

	for (int i = 0; i < _sizeX * _sizeY; i++)
	{
		_data[i].second.clear();
		_data[i].first = 0;
	}

	for (int i = 0; i < particleCount; i++) {
		float* p = particles[i];
		for (int x = max(1, (int)ceil((p[X] - rc) / cubeSize)); x <= min((double)_sizeX-2, floor((p[X] + rc) / cubeSize)); x++) {
			double rdy = rc*rc - (p[X] - x*cubeSize)*(p[X] - x*cubeSize);
			for (int y = max(1, (int)ceil((p[Y] - rdy) / cubeSize)); y <= min((double)_sizeY-2, floor((p[Y] + rdy) / cubeSize)); y++) {
				double rdz = rdy*rdy - (p[Y] - y*cubeSize)*(p[Y] - y*cubeSize);
				get(x, y).second.push_back(
					ParticleLookupCacheEntry(
						max(1,      (int)ceil((p[Z] - rdz) / cubeSize)),
						min(_sizeZ-2, (int)floor((p[Z] + rdz) / cubeSize)),
						floor(p[Z] / cubeSize),
						p
					)
				);
			}
		}
	}
}

void ParticleLookupCache::init(float** particles, int particleCount, int particleSize)
{
	sort(particles, particles + particleCount, ParticleComp(2));

	for (int i = 0; i < _sizeX * _sizeY; i++)
	{
		_data[i].second.clear();
		_data[i].first = 0;
	}

	for (int i = 0; i < particleCount; i++) {
		float* p = particles[i];
		float px = p[X];
		float py = p[Y];
		float pz = p[Z];
		for (int x = max(1, (int)ceil((p[X] - _rc - _xMin) / _cubeSize)); x <= min((double)_sizeX-2, floor((p[X] + _rc - _xMin) / _cubeSize)); x++) {
			double rdy = _rc*_rc - (p[X] - (_xMin + x*_cubeSize))*(p[X] - (_xMin + x*_cubeSize));
			for (int y = max(1, (int)ceil((p[Y] - rdy - _yMin) / _cubeSize)); y <= min((double)_sizeY-2, floor((p[Y] + rdy - _yMin) / _cubeSize)); y++) {
				double rdz = rdy*rdy - (p[Y] - (_yMin + y*_cubeSize))*(p[Y] - (_yMin + y*_cubeSize));
				ListType& column = get(x, y).second;
				/*
				get(x, y).second.push_back(
					ParticleLookupCacheEntry(
						max(1,      (int)ceil((p[Z] - rdz - _zMin) / _cubeSize)),
						min(_sizeZ-2, (int)floor((p[Z] + rdz - _zMin) / _cubeSize)),
						floor((p[Z] - _zMin) / _cubeSize),
						p
					)
				);
				*/
				get(x, y).second.push_back(
					ParticleLookupCacheEntry(
						max(1,      (int)floor((p[Z] - rdz - _zMin) / _cubeSize)),
						min(_sizeZ-2, (int)ceil((p[Z] + rdz - _zMin) / _cubeSize)),
						floor((p[Z] - _zMin) / _cubeSize),
						p
					)
				);
			}
		}
	}
}

#define SQRT2 1.4142135623730

double ParticleLookupCache::getFieldValueAt(int x, int y, int z)
{
	double retVal = 0.0;
	const double FAC = SQRT2 * _rc;
	int rc_int = ceil(_rc / _cubeSize);
	ListType& column = get(x, y).second;

	int i = get(x, y).first;
	while (i > 0 && column[i].midZ >= rc_int) 
		i--;
	while (i < column.size() && column[i].midZ < z - rc_int)
		i++;

	get(x, y).first = std::max<int>(0, std::min<int>(column.size()-1, i));

	double xx = _xMin + x * _cubeSize;
    double yy = _yMin + y * _cubeSize;
    double zz = _zMin + z * _cubeSize;
	while (i != column.size() && column[i].midZ <= z + rc_int) {
		if (column[i].minZ <= z && column[i].maxZ >= z) {
			double r = sqrt(
				(column[i].particle[X] - xx) * (column[i].particle[X] - xx) +
				(column[i].particle[Y] - yy) * (column[i].particle[Y] - yy) +
				(column[i].particle[Z] - zz) * (column[i].particle[Z] - zz));
			double tmp = r / FAC;
			tmp *= tmp;
			retVal += tmp*tmp - tmp + 0.25;
		}
		i++;
	}

	return retVal;
}

double ParticleLookupCache::getFieldValueAt2(int x, int y, int z)
{
	double retVal = 0.0;
	const double FAC = 2 * _rc * _rc;
	int rc_int = ceil(_rc / _cubeSize);
	ListType& column = get(x, y).second;

	int i = 0;
	while (i < column.size() && column[i].midZ < z - rc_int)
		i++;

    double xx = _xMin + x * _cubeSize;
    double yy = _yMin + y * _cubeSize;
    double zz = _zMin + z * _cubeSize;
	while (i < column.size() && column[i].midZ <= z + rc_int) {
		if (column[i].minZ <= z && column[i].maxZ >= z) {
			double r =
				(column[i].particle[X] - xx) * (column[i].particle[X] - xx) +
				(column[i].particle[Y] - yy) * (column[i].particle[Y] - yy) +
				(column[i].particle[Z] - zz) * (column[i].particle[Z] - zz);
			double tmp = r / FAC;
			retVal += tmp*tmp - tmp + 0.25;
		}
		i++;
	}

	return retVal;
}
