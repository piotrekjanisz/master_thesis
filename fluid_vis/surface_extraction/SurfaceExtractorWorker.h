#pragma once

#include "IWorkCoordinator.h"
#include "SurfaceExtractor.h"
#include "data_types.h"
#include <boost/shared_ptr.hpp>

class SurfaceExtractorWorker
{
public:
	SurfaceExtractorWorker(const SurfaceExtractorDesc& desc, IWorkCoordinator* coordinator);

	~SurfaceExtractorWorker();

	void operator()();

private: // classes
	class Buffer
	{
		float* _verticesBuffer;
		float* _normalsBuffer;
		unsigned int* _trianglesBuffer;

		int _maxVertices;
		int _maxTriangles;

		int _verticesCount;
		int _trianglesCount;

	public:
		Buffer(int maxVertices, int maxTriangles)
			: _maxVertices(maxVertices), _maxTriangles(maxTriangles), _verticesCount(0), _trianglesCount(0)
		{
			_verticesBuffer = new float[_maxVertices * 4];
			_normalsBuffer = new float[_maxVertices * 3];
			_trianglesBuffer = new unsigned int[_maxTriangles * 3];
			for (int i = 0; i < _maxVertices; i++) {
				_verticesBuffer[i * 4 + 3] = 1.0f;
			}
		}

		~Buffer()
		{
			if (_verticesBuffer) delete [] _verticesBuffer;
			if (_normalsBuffer) delete [] _normalsBuffer;
			if (_trianglesBuffer) delete [] _trianglesBuffer;
		}

		void advance(int ntriag, int nvert)
		{
			_verticesCount += nvert;
			_trianglesCount += ntriag;
		}

		float* getVerticesBuffer()
		{
			return &_verticesBuffer[_verticesCount * 4];
		}

		float* getNormalsBuffer()
		{
			return &_normalsBuffer[_verticesCount * 3];
		}

		unsigned int* getTrianglesBuffer()
		{
			return &_trianglesBuffer[_trianglesCount * 3];
		}

		void reset()
		{
			_trianglesCount = 0;
			_verticesCount = 0;
		}
	};

private: // methods
	void reset();

	void rotateBuffers()
	{
		_currentBuffer = (_currentBuffer + 1) % 2;
	}

	boost::shared_ptr<Buffer>& currentBuffer()
	{
		return _buffers[_currentBuffer];
	}

private: // fields
	boost::shared_ptr<Buffer> _buffers[2];
	int _currentBuffer;

	SurfaceExtractor _surfaceExtractor;

	IWorkCoordinator* _coordinator;
};
