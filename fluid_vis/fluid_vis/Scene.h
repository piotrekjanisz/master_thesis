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
	int _inverseProjectionLocation;

	int _quadProjectionLocation;
	int _quadModelViewLocation;

	int _skyBoxProjectionLocation;
	int _skyBoxModelViewLocation;

	ShaderProgramPtr _shaderProgram;
	ShaderProgramPtr _waterShader;
	ShaderProgramPtr _waterDepthShader;
	ShaderProgramPtr _smoothShader;
	ShaderProgramPtr _finalShader;
	ShaderProgramPtr _skyBoxShader;
	GfxStaticObjectPtr _box;
	GfxStaticObjectPtr _plane;
	GfxStaticObjectPtr _skyBox;
	TexturePtr _sceneDepthTexture;
	TexturePtr _sceneTexture;
	TexturePtr _screenQuadTexture;
	TexturePtr _depthTexture;
	TexturePtr _zTexture;
	TexturePtr _smoothedTexture;
	TexturePtr _gaussDistTexture;
	TexturePtr _spatialDistTexture;

	TexturePtr _boxTexture;
	TexturePtr _floorTexture;
	TexturePtr _skyBoxTexture;

	FrameBufferPtr _sceneFrameBuffer;
	FrameBufferPtr _waterFrameBuffer;
	FrameBufferPtr _smoothFrameBuffer;
	ScreenQuadPtr _screenQuad;
	ScreenQuadPtr _finalQuad;

	MyFluid* _fluid;

	float* _debugData;

public:
	Scene();

	virtual bool setup();
	virtual void render();
	void render(NxScene* physicsScene);
};

