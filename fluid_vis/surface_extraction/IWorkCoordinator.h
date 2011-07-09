#pragma once

#include "Block.h"
#include "data_types.h"

class IWorkCoordinator
{
public:
	virtual Block* getNextBlock() = 0;
	virtual void submitMesh(const TriangleMesh& mesh) = 0;
};
