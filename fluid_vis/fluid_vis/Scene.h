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
private: // fields
	int _projectionLocation;
	int _modelViewLocation;
	int _normalMatrixLocation;
	int _colorLocation;
	int _inverseProjectionLocation;
	int _mvpLocation;

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
	ShaderProgramPtr _normalMapShader;
	ShaderProgramPtr _isoSurfaceProgram;
	ShaderProgramPtr _waterShader;
	ShaderProgramPtr _waterDepthShader;
	ShaderProgramPtr _smoothShader;
	ShaderProgramPtr _finalShader;
	ShaderProgramPtr _skyBoxShader;
	ShaderProgramPtr _gaussianBlurShader;
	ShaderProgramPtr _grayscaleTextureShader;

	ShaderProgramPtr _bilateralGaussSmoothShader;
	ShaderProgramPtr _curvatureFlowShader;
	ShaderProgramPtr _edgeDetectionShader;
	
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
	TexturePtr _smoothedTexture2;
	TexturePtr _gaussDistTexture;
	TexturePtr _gaussDist1DTexture;
	TexturePtr _gaussDist1DBilateralTexture;
	TexturePtr _spatialDistTexture;
	TexturePtr _gaussDistributionsArrayTexture;
	TexturePtr _edgeTexture;

	TexturePtr _boxTexture;
	TexturePtr _floorTexture;
	TexturePtr _floorNormalMapTexture;
	TexturePtr _skyBoxTexture;

	FrameBufferPtr _sceneFrameBuffer;
	FrameBufferPtr _waterFrameBuffer;
	FrameBufferPtr _smoothFrameBuffer;
	FrameBufferPtr _waterDepthFrameBuffer;
	ScreenQuadPtr _screenQuad;
	ScreenQuadPtr _blurQuad;
	ScreenQuadPtr _finalQuad;
	ScreenQuadPtr _grayscaleIntermediateQuad;
	ScreenQuadPtr _edgeQuad;
	ScreenQuadPtr _curvatureFlowQuad;

	MyFluid* _fluid;

	boost::shared_ptr<Block> _block;

	float* _debugData;

	int _particleCount;

	float _particleSize;

	SurfaceExtractorPtr _surfaceExtractor;
	MtSurfaceExtractorPtr _mtSurfaceExtractor;

	std::list<TriangleMesh> _outputs[2];
	int _currentOutput;

	double _bilateralTreshold;
	int _bilateralGaussSize;
	double _bilateralGaussSigma;
	int _additionalBlurPhases;

	double _depthGaussSigma;
	int _depthGaussSize;

	float _particleDepth;

	float _filterSizeMult;
	int _maxFilter;

	float _timeStep;
	float _timeStepChange;

	float _edgeTreshold;
	float _edgeTresholdChange;

	vmml::vec4f _lightDirection;

private: // methods
	bool setupTextures();
	bool setupFramebuffers();
	bool setupShaders();
	bool setupObjects();

public:
	Scene();

	virtual bool setup();
	virtual void render();

	void changeBilateralTreshold(double change);
	void changeGauss(int sizeChange, double sigmaChange);
	void changeDepthGauss(int sizeChange, double sigmaChange);
	void changeAdditionalBlurPhases(int change);
	void changeParticleDepth(float change);
	void changeFilterSizeMult(float change);
	void changeParticleSize(float change);
	void changeTimeStep(float change);
	void changeEdgeTreshold(float change);

	void rotateLightDir(float xrot, float yrot);

	void render(NxScene* physicsScene);
	void renderIsoSurface(NxScene* physicsScene);

	virtual void reshape(int width, int height);

	virtual void displayAdditionalStats();

	void incParticleSize(float val) {
		_particleSize = max(_particleSize + val, 10.0f);
	}

	vmml::vec4f getLightInEyeSpace()
	{
		return _viewMatrix * _lightDirection;
	}
};

