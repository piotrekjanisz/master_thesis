#pragma once

#include <NxPhysics.h>
#include "ShaderProgram.h"
#include "AbstractScene.h"
#include "GfxStaticObject.h"
#include <vmmlib\vmmlib.hpp>
#include <boost\smart_ptr.hpp>

class Scene : public AbstractScene
{	
	int _projectionLocation;
	int _modelViewLocation;
	int _normalMatrixLocation;
	int _colorLocation;

	boost::shared_ptr<ShaderProgram> _shaderProgram;
	boost::shared_ptr<GfxStaticObject> _box;
	boost::shared_ptr<GfxStaticObject> _plane;

public:
	Scene();

	virtual bool setup();
	virtual void render();
	void render(NxScene* physicsScene);
};

