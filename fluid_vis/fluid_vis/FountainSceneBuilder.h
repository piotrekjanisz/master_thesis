#pragma once

#include "physxscenebuilder.h"

class FountainSceneBuilder :
	public PhysXSceneBuilder
{
public:
	FountainSceneBuilder(void);
	~FountainSceneBuilder(void);

	virtual void buildScene(NxScene* nxScene, ConfigurationFactory& configurationFactory, bool accelerateFluid);
};

