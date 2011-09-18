#include "ParticleLookupCache.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <utils/mymath.h>

using namespace std;

#define X 0
#define Y 1
#define Z 2

ParticleLookupCache::ParticleLookupCache(int sizeX, int sizeY, int sizeZ)
	: _sizeX(sizeX), _sizeY(sizeY), _sizeZ(sizeZ)
{
	_data = new std::pair<int, ListType>[sizeX * sizeY];
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

void ParticleLookupCache::init(Block& block)
{
	_xMin = block.xMin;
	_xMax = block.xMax;
	_yMin = block.yMin;
	_yMax = block.yMax;
	_zMin = block.zMin;
	_zMax = block.zMax;

	_rc = block.rc;
	_cubeSize = block.cubeSize;

	sort(block.particles, block.particles + block.particlesCount, ParticleComp(2));

	for (int i = 0; i < _sizeX * _sizeY; i++)
	{
		_data[i].second.clear();
		_data[i].first = 0;
	}

	for (int i = 0; i < block.particlesCount; i++) {
		float* p = block.particles[i];
		float px = p[X];
		float py = p[Y];
		float pz = p[Z];
		for (int x = max(1, (int)ceil((p[X] - _rc - _xMin) / _cubeSize)); x <= min((double)_sizeX-2, floor((p[X] + _rc - _xMin) / _cubeSize)); x++) {
			double rdy = sqrt(_rc*_rc - (p[X] - (_xMin + x*_cubeSize))*(p[X] - (_xMin + x*_cubeSize)));
			for (int y = max(1, (int)ceil((p[Y] - rdy - _yMin) / _cubeSize)); y <= min((double)_sizeY-2, floor((p[Y] + rdy - _yMin) / _cubeSize)); y++) {
				double rdz = sqrt(rdy*rdy - (p[Y] - (_yMin + y*_cubeSize))*(p[Y] - (_yMin + y*_cubeSize)));
				ListType& column = get(x, y).second;
				get(x, y).second.push_back(
					ParticleLookupCacheEntry(
						max(1,      (int)ceil((p[Z] - rdz - _zMin) / _cubeSize)),
						min(_sizeZ-2, (int)floor((p[Z] + rdz - _zMin) / _cubeSize)),
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

double ParticleLookupCache::getFieldValueAndNormalAt(int x, int y, int z, float* normal)
{
	normal[0] = 0;
	normal[1] = 0;
	normal[2] = 0;

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
			float r_vec[3];
			MyMath::vec_create(r_vec, xx, yy, zz,
				column[i].particle[X], column[i].particle[Y], column[i].particle[Z]);

			double r = MyMath::vec_len_square(r_vec);
			double tmp = r / FAC;
			double val = tmp*tmp - tmp + 0.25;
			retVal += val;

			if (r > 0) {
				MyMath::vec_normalize(r_vec);
			}
			MyMath::vec_add_scaled(normal, r_vec, val);
		}
		i++;
	}

	if (retVal > 0) {
		MyMath::vec_divide(normal, retVal);
	}

	return retVal;
}
