#pragma once

#include "physxscenebuilder.h"

class BasinSceneBuilder :
	public PhysXSceneBuilder
{
public:
	BasinSceneBuilder(void);
	~BasinSceneBuilder(void);

	virtual void buildScene(NxScene* nxScene, ConfigurationFactory& configurationFactory, bool accelerateFluid);
};

