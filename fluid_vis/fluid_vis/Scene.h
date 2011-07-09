#pragma once

#include <NxPhysics.h>
#include "ShaderProgram.h"
#include "AbstractScene.h"
#include "GfxStaticObject.h"
#include "GfxObject.h"
#include "MyFluid.h"
#include "FrameBuffer.h"
#include "data_types.h"
#include "Texture.h"
#include "ScreenQuad.h"
#include "ShapeFactory.h"
#include <surface_extraction/SurfaceExtractor.h>
#include <surface_extraction/MtSurfaceExtractor.h>
#include <utils/utils.h>
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

	int _waterProjectionLocation;
	int _waterModelViewLocation;

	int _waterDepthProjectionLocation;
	int _waterDepthModelViewLocation;

	int _isoWaterProjectionLocation;
	int _isoWaterModelViewLocation;
	int _isoWaterNormalLocation;

	ShaderProgramPtr _shaderProgram;
	ShaderProgramPtr _isoSurfaceProgram;
	ShaderProgramPtr _waterShader;
	ShaderProgramPtr _waterDepthShader;
	ShaderProgramPtr _smoothShader;
	ShaderProgramPtr _finalShader;
	ShaderProgramPtr _skyBoxShader;
	ShaderProgramPtr _gaussianBlurShader;
	
	GfxStaticObjectPtr _box;
	GfxStaticObjectPtr _plane;
	GfxStaticObjectPtr _skyBox;
	GfxObjectPtr _water;
	GfxObjectPtr _isoWater;

	TexturePtr _sceneDepthTexture;
	TexturePtr _sceneTexture;
	TexturePtr _screenQuadTexture;
	TexturePtr _depthTexture;
	TexturePtr _waterDepthTexture;
	TexturePtr _zTexture;
	TexturePtr _smoothedTexture;
	TexturePtr _gaussDistTexture;
	TexturePtr _gaussDist1DTexture;
	TexturePtr _gaussDist1DBilateralTexture;
	TexturePtr _spatialDistTexture;

	TexturePtr _boxTexture;
	TexturePtr _floorTexture;
	TexturePtr _skyBoxTexture;

	FrameBufferPtr _sceneFrameBuffer;
	FrameBufferPtr _waterFrameBuffer;
	FrameBufferPtr _smoothFrameBuffer;
	FrameBufferPtr _waterDepthFrameBuffer;
	ScreenQuadPtr _screenQuad;
	ScreenQuadPtr _blurQuad;
	ScreenQuadPtr _finalQuad;

	MyFluid* _fluid;

	boost::shared_ptr<Block> _block;

	float* _debugData;

	int _particleCount;

	float _particleSize;

	SurfaceExtractorPtr _surfaceExtractor;
	MtSurfaceExtractorPtr _mtSurfaceExtractor;

	std::list<TriangleMesh> _outputs[2];
	int _currentOutput;

public:
	Scene();

	virtual bool setup();
	virtual void render();
	void render(NxScene* physicsScene);
	void renderIsoSurface(NxScene* physicsScene);

	virtual void reshape(int width, int height);

	virtual void displayAdditionalStats();

	void incParticleSize(float val) {
		_particleSize = max(_particleSize + val, 10.0f);
	}
};

