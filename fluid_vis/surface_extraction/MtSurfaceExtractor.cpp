#include "MtSurfaceExtractor.h"
#include <cmath>
#include <cstdlib>
#include <boost/make_shared.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/thread/condition_variable.hpp>

using namespace std;

MtSurfaceExtractor::MtSurfaceExtractor(const SurfaceExtractorDesc& desc)
	: _desc(desc), _workers(0), _threads(0), _running(true)
{
	_blocksX = ceil((_desc.xMax - _desc.xMin) / _desc.blockSize);
	_blocksY = ceil((_desc.yMax - _desc.yMin) / _desc.blockSize);
	_blocksZ = ceil((_desc.zMax - _desc.zMin) / _desc.blockSize);

	_blocks.allocate(_blocksX, _blocksY, _blocksZ);

	for (int x = 0; x < _blocksX; x++) {
		for (int y = 0; y < _blocksY; y++) {
			for (int z = 0; z < _blocksZ; z++) {
				_blocks(x, y, z) = new Block(
					_desc.maxParticles, 
					_desc.xMin + x * _desc.blockSize - _desc.rc, desc.xMin + (x+1) * _desc.blockSize + _desc.rc,
					_desc.yMin + y * _desc.blockSize - _desc.rc, desc.yMin + (y+1) * _desc.blockSize + _desc.rc,
					_desc.zMin + z * _desc.blockSize - _desc.rc, desc.zMin + (z+1) * _desc.blockSize + _desc.rc,
					_desc.rc, _desc.cubeSize
				);
			}
		}
	}

	initNbrs();

	int xBlockSize = _blocks(0, 0, 0)->xSize;
	int yBlockSize = _blocks(0, 0, 0)->ySize;
	int zBlockSize = _blocks(0, 0, 0)->zSize;

	_workers = new boost::shared_ptr<SurfaceExtractorWorker>[_desc.threads];
	for (int i = 0; i < _desc.threads; i++) {
		// FIXME what if xBlockSize != yBlockSize
		_workers[i] = boost::make_shared<SurfaceExtractorWorker>(desc, this);
	}

	_threads = new boost::thread[desc.threads];
	for (int i = 0; i < _desc.threads; i++) {
		_threads[i] = boost::thread(boost::ref(*_workers[i]));
	}
}

MtSurfaceExtractor::~MtSurfaceExtractor()
{
	_running = false;
	_currentBlock = _blocks.size() + 1;

	for (int i = 0; i < _desc.threads; i++) {
		_workers[i]->start();
		_threads[i].join();
	}

	if (_workers)
		delete [] _workers;
	if (_threads)
		delete [] _threads;
}

void MtSurfaceExtractor::initNbrs()
{
	for (int i = 0; i < 64; i++) {	
		int x = 0;
		int y = 0;
		int z = 0;

		if (i & 1)
			x = -1;
		if (i & 2)
			x = 1;
		if (i & 4)
			y = -1;
		if (i & 8)
			y = 1;
		if (i & 16)
			z = -1;
		if (i & 32)
			z = 1;

		Vec3 v(0, 0, 0);

		for (int xx = 0; xx < abs(x)+1; xx++) {
			for (int yy = 0; yy < abs(y)+1; yy++) {
				for (int zz = 0; zz < abs(z)+1; zz++) {
					Vec3 v(xx * x, yy * y, zz * z);
					if (v.x || v.y || v.z)
					_nbrs[i].push_back(v);
				}
			}
		}
	}
}

void MtSurfaceExtractor::extractSurface(float* particles, int particlesCount, int particleComponents, list<TriangleMesh>* output)
{
	_meshes = output;

	for (int i = 0; i < _blocks.size(); i++) {
		_blocks(i)->clearParticles();
	}

	// initialize blocks - redistribute particles among them
	for (int i = 0; i < particlesCount; i++) {
		float* particle = &particles[i * particleComponents];
		int bx = (particle[0] - _desc.xMin) / _desc.blockSize;
		int by = (particle[1] - _desc.yMin) / _desc.blockSize;
		int bz = (particle[2] - _desc.zMin) / _desc.blockSize;
		if (bx < 0 || by < 0 || bz < 0 ||
			bx >= _blocksX || by >= _blocksY || bz >= _blocksZ)
			continue;

		Block* block = _blocks(bx, by, bz);
		block->addParticles(particle);

		int nbrIndex = 0;
		if ((particle[0] - block->xMin) <= 2*block->rc && bx > 0)
			nbrIndex |= 1;
		if ((block->xMax - particle[0]) <= 2*block->rc && bx < _blocksX-1)
			nbrIndex |= 2;
		if ((particle[1] - block->yMin) <= 2*block->rc && by > 0)
			nbrIndex |= 4;
		if ((block->yMax - particle[1]) <= 2*block->rc && by < _blocksY-1)
			nbrIndex |= 8;
		if ((particle[2] - block->zMin) <= 2*block->rc && bz > 0)
			nbrIndex |= 16;
		if ((block->zMax - particle[2]) <= 2*block->rc && bz < _blocksZ-1)
			nbrIndex |= 32;

		for (int j = 0; j < _nbrs[nbrIndex].size(); j++) {
			_blocks(bx + _nbrs[nbrIndex][j].x, by + _nbrs[nbrIndex][j].y, bz + _nbrs[nbrIndex][j].z)->addParticles(particle);
		}
	}

	_currentBlock = 0;

	// fire workers
	for (int i = 0; i < _desc.threads; i++) {
		_workers[i]->start();
	}
}

bool MtSurfaceExtractor::isRunning()
{
	return _running;
}

Block* MtSurfaceExtractor::getNextBlock()
{
	boost::interprocess::scoped_lock<boost::mutex> lock(_blocksMutex);

	Block* retVal = 0;

	if (_currentBlock < _blocks.size()) {
		retVal = _blocks(_currentBlock);
		_currentBlock++;
	}

	return retVal;
}

void MtSurfaceExtractor::submitMesh(const TriangleMesh& mesh)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(_meshesMutex);
	_meshes->push_back(mesh);
}

void MtSurfaceExtractor::waitForResults()
{
	for (int i = 0; i < _desc.threads; i++) {
		_workers[i]->wait();
	}
}


