#pragma once

#include "Slice.h"
#include "ParticleLookupCache.h"
#include "Block.h"
#include "data_types.h"
#include "SurfaceExtractorWorker.h"
#include "SurfaceExtractor.h"
#include "IWorkCoordinator.h"
#include <utils/utils.h>
#include <vector>
#include <list>
#include <set>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>


class MtSurfaceExtractor : public IWorkCoordinator
{
	struct Vec3
	{
		Vec3(int _x, int _y, int _z)
			:x(_x), y(_y), z(_z) {}
		int x; int y; int z;
	};

	std::vector<Vec3> _nbrs[64];

	Utils::Linear3dArray<Block*> _blocks;

	int _blocksX;
	int _blocksY;
	int _blocksZ;

	SurfaceExtractorDesc _desc;

	boost::shared_ptr<SurfaceExtractorWorker>* _workers;
	boost::thread* _threads;

	int _currentBlock;
	std::list<TriangleMesh>* _meshes;

	boost::mutex _meshesMutex;
	boost::mutex _blocksMutex;

	bool _running;
private: //methods
	void initNbrs();

public:

	struct Request
	{
		float* particles;
		int particlesCount;
		int particleComponents;

		float* verticesBuffer;
		float* normalsBuffer;
		unsigned int* indicesBuffer;

		int maxVertices;
		int maxTriangles;
	};

	MtSurfaceExtractor(const SurfaceExtractorDesc& desc);

	void extractSurface(float* particles, int particesCount, int particleComponents, std::list<TriangleMesh>* output);

	// blocks until extractSurface function results are ready
	void waitForResults();

	~MtSurfaceExtractor();

	virtual Block* getNextBlock();
	virtual void submitMesh(const TriangleMesh& mesh);
	virtual bool isRunning();
};

