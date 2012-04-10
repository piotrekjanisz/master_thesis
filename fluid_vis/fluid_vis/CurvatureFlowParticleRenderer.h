#pragma once

#include <utils/ParameterController.h>
#include "ParticleRenderer.h"
#include "FrameBuffer.h"
#include "GfxObject.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "data_types.h"


/**
 TODO:
	- create ScreenSpaceParticleRenderer class
	- add virtual method getParameters
*/

class CurvatureFlowParticleRenderer : public ParticleRenderer
{
private: // constants
	static const std::string PARAM_PARTICLE_SIZE;
	static const std::string PARAM_THICKNESS_GAUSS_SIZE;
	static const std::string PARAM_THICKNESS_GAUSS_SIGMA;
	static const std::string PARAM_BLUR_ITERATION_COUNT;
	static const std::string PARAM_PARTICLE_THICKNESS;
	static const std::string PARAM_TIME_STEP;
	static const std::string PARAM_EDGE_TRESHOLD;
	static const std::string PARAM_THICKNESS_SIZE;

private: // auxiliary functions
	bool setupShaders();
	bool setupObjects();
	bool setupFramebuffers();
	bool setupTextures();

	void setupThicknessGauss();

private: // rendering parameters
	std::set<std::string> _parameterNames;

	float _particleSize;
	float _particleDepth;
	float _edgeTreshold;
	float _timeStep;
	int _blurIterationCount;

	float _thicknessGaussSigma;
	int _thicknessGaussSize;

	float _thicknessSize;

private: // graphic objects
	AbstractScene* _scene;

	// framebuffers
	FrameBufferPtr _waterThicknessFB;
	FrameBufferPtr _smoothFB;
	FrameBufferPtr _waterFB;

	// textures
	TexturePtr _waterThicknessTexture;
	TexturePtr _waterLinDetphTexture;
	TexturePtr _waterDepthTexture;
	TexturePtr _smoothedTexture;
	TexturePtr _smoothedTexture2;
	TexturePtr _gaussDist1DTexture;
	TexturePtr _edgeTexture;
	TexturePtr _thicknessAuxTexture;

	// objects
	GfxObjectPtr _water;

	// screenQuads
	ScreenQuadPtr _blurQuad;
	ScreenQuadPtr _finalQuad;
	ScreenQuadPtr _edgeQuad;
	ScreenQuadPtr _curvatureFlowQuad;
	ScreenQuadPtr _fillDepthBufferQuad;

	// shaders
	ShaderProgramPtr _waterThicknessShader;
	ShaderProgramPtr _waterShader;
	ShaderProgramPtr _gaussianBlurShader;
	ShaderProgramPtr _finalShader;
	ShaderProgramPtr _edgeDetectionShader;
	ShaderProgramPtr _curvatureFlowShader;
	ShaderProgramPtr _copyTextureToDepthShader;

	// debug stuff
	ShaderProgramPtr _grayscaleTextureShader;
	ScreenQuadPtr _grayscaleIntermediateQuad;
public:
	CurvatureFlowParticleRenderer(AbstractScene* scene);
	virtual ~CurvatureFlowParticleRenderer(void);

	bool setup();
	virtual void resize(int width, int height);
	virtual void render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData);
	virtual const std::set<std::string>& getParameters();
	virtual bool changeParameter(const std::string& parameter, ParamOperation operation);
};

