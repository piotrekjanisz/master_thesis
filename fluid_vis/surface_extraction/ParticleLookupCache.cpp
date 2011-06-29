#include "ParticleLookupCache.h"
#include <algorithm>

using namespace std;

#define X 0
#define Y 1
#define Z 2

ParticleLookupCache::ParticleLookupCache(int sizeX, int sizeY, int sizeZ)
	: _sizeX(sizeX), _sizeY(sizeY), _sizeZ(sizeZ)
{
	_data = new list<ParticleLookupCacheEntry>*[sizeX];
	for (int i = 0; i < sizeX; i++) {
		_data[i] = new list<ParticleLookupCacheEntry>[sizeY];
	}
}

ParticleLookupCache::~ParticleLookupCache()
{
	if (_data) {
		for (int i = 0; i < _sizeX; i++) {
			delete [] _data[i];
		}
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

	for (int i = 0; i < particleCount; i++) {
		float* p = particles[i];
		for (int x = max(1, (int)ceil((p[X] - rc) / cubeSize)); x <= min((double)_sizeX-2, floor((p[X] + rc) / cubeSize)) & x < _sizeX; x++) {
			double rdy = rc*rc - (p[X] - x*cubeSize)*(p[X] - x*cubeSize);
			for (int y = max(1, (int)ceil((p[Y] - rdy) / cubeSize)); y <= min((double)_sizeY-2, floor((p[Y] + rdy) / cubeSize)) && y < _sizeY; y++) {
				double rdz = rdy*rdy - (p[Y] - y*cubeSize)*(p[Y] - y*cubeSize);
				_data[x][y].push_back(
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

#define SQRT2 1.4142135623730

double ParticleLookupCache::getFieldValueAt(int x, int y, int z)
{
	double retVal = 0.0;
	const double FAC = SQRT2 * _rc;

	list<ParticleLookupCacheEntry>::iterator it = _data[x][y].begin();
	while (it != _data[x][y].end() && it->midZ < z - ceil(_rc / _cubeSize))
		++it;

	while (it != _data[x][y].end() && it->midZ <= z + ceil(_rc / _cubeSize)) {
		if (it->minZ <= z && it->maxZ >= z) {
			double r = sqrt(
				(it->particle[X] - x*_cubeSize) * (it->particle[X] - x*_cubeSize) +
				(it->particle[Y] - y*_cubeSize) * (it->particle[Y] - y*_cubeSize) +
				(it->particle[Z] - z*_cubeSize) * (it->particle[Z] - z*_cubeSize));
			double tmp = r / FAC;
			tmp *= tmp;
			retVal += tmp*tmp - tmp + 0.25;
		}
		++it;
	}

	// TODO save it position to speedup further searches

	return retVal;
}