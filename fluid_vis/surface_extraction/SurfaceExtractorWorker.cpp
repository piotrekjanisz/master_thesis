#include "SurfaceExtractorWorker.h"
#include <boost/make_shared.hpp>

SurfaceExtractorWorker::SurfaceExtractorWorker(const SurfaceExtractorDesc& desc, IWorkCoordinator* coordinator)
	: _coordinator(coordinator), _surfaceExtractor(desc)
{
	for (int i = 0; i < 2; i++) {
		_buffers[i] = boost::make_shared<Buffer>(desc.maxVerticesPerThread, desc.maxTrianglesPerThread);
	}
	_currentBuffer = 0;
}

SurfaceExtractorWorker::~SurfaceExtractorWorker()
{
}

void SurfaceExtractorWorker::operator()()
{
	Block* block;

	currentBuffer()->reset();

	while (block = _coordinator->getNextBlock()) {
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
}
