#pragma once

#include <NxPhysics.h>
#include "ShaderProgram.h"
#include "AbstractScene.h"
#include "GfxStaticObject.h"
#include "MyFluid.h"
#include "FrameBuffer.h"
#include "data_types.h"
#include "Texture.h"
#include "ScreenQuad.h"
#include <vmmlib\vmmlib.hpp>
#include <boost\smart_ptr.hpp>

class Scene : public AbstractScene
{	
	int _projectionLocation;
	int _modelViewLocation;
	int _normalMatrixLocation;
	int _colorLocation;

	int _quadProjectionLocation;
	int _quadModelViewLocation;

	ShaderProgramPtr _shaderProgram;
	GfxStaticObjectPtr _box;
	GfxStaticObjectPtr _plane;
	TexturePtr _screenQuadTexture;
	FrameBufferPtr _sceneFrameBuffer;
	ScreenQuadPtr _screenQuad;

	MyFluid* _fluid;

	float* _debugData;

public:
	Scene();

	virtual bool setup();
	virtual void render();
	void render(NxScene* physicsScene);
};

