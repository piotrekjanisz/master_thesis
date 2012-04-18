#include "SurfaceExtractorWorker.h"
#include <boost/make_shared.hpp>

SurfaceExtractorWorker::SurfaceExtractorWorker(const SurfaceExtractorDesc& desc, IWorkCoordinator* coordinator)
	: _coordinator(coordinator), _surfaceExtractor(desc), _isRunning(false)
{
	for (int i = 0; i < 2; i++) {
		_buffers[i] = boost::make_shared<Buffer>(desc.maxVerticesPerThread, desc.maxTrianglesPerThread);
	}
	_currentBuffer = 0;
}

SurfaceExtractorWorker::~SurfaceExtractorWorker()
{
}

void SurfaceExtractorWorker::wait()
{
	boost::mutex::scoped_lock lock(_isRunningMutex);
    while(_isRunning)
	{
		_isRunningCondition.wait(lock);
    }
}

void SurfaceExtractorWorker::notifyDone()
{
	{
		boost::mutex::scoped_lock lock(_isRunningMutex);
		_isRunning = false;
	}
	_isRunningCondition.notify_all();
}


void SurfaceExtractorWorker::start()
{
	bool wasStarted = false;
	{
		boost::mutex::scoped_lock lock(_isRunningMutex);
		wasStarted = _isRunning;
		_isRunning = true;
	}
	if (!wasStarted)
		_isRunningCondition.notify_all();
}

void SurfaceExtractorWorker::waitForStart()
{
	boost::mutex::scoped_lock lock(_isRunningMutex);
	while(!_isRunning) {
		_isRunningCondition.wait(lock);
	}
}

void SurfaceExtractorWorker::operator()()
{
	while (_coordinator->isRunning()) {	
		waitForStart();

		Block* block;

		currentBuffer()->reset();

		while (block = _coordinator->getNextBlock()) {
			if (block->particlesCount <= 0)
				continue;
			int nvert;
			int ntriag;
			_surfaceExtractor.extractSurface(*block, 4, 
				currentBuffer()->getVerticesBuffer(),
				currentBuffer()->getNormalsBuffer(),
				currentBuffer()->getTrianglesBuffer(),
				nvert, ntriag, 11111111, 1111111);

			TriangleMesh mesh;
			mesh.vertices = currentBuffer()->getVerticesBuffer();
			mesh.normals = currentBuffer()->getNormalsBuffer();
			mesh.indices = currentBuffer()->getTrianglesBuffer();
			mesh.verticesCount = nvert;
			mesh.trianglesCount = ntriag;

			_coordinator->submitMesh(mesh);

			_buffers[_currentBuffer]->advance(ntriag, nvert);
		}

		rotateBuffers();
		notifyDone();
	}
}
