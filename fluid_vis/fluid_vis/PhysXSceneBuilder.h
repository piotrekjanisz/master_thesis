#pragma once

#include <NxPhysics.h>

#include "ConfigurationFactory.h"

class PhysXSceneBuilder
{
public:
	PhysXSceneBuilder(void);
	virtual ~PhysXSceneBuilder(void);

	virtual void buildScene(NxScene* nxScene, ConfigurationFactory& configurationFactory, bool accelerateFluid) = 0;

	void createStaticBox(NxScene* nxScene, const NxVec3& pos, const NxVec3& dim);

	void createCube(NxScene* nxScene, const NxVec3& pos, float size = 1.0f, const NxVec3& initialVelocity = NxVec3(0.0f, 0.0f, 0.0f));

	void createCubeFromEye(NxScene* nxScene, const NxVec3& position, const NxVec3& velocity, float size);
};

