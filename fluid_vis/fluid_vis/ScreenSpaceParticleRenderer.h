#pragma once

#include "ParticleRenderer.h"
#include "FrameBuffer.h"
#include "GfxObject.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "data_types.h"

class ScreenSpaceParticleRenderer : public ParticleRenderer
{
private: // constants
	static const std::string PARAM_PARTICLE_SIZE;
	static const std::string PARAM_THICKNESS_GAUSS_SIZE;
	static const std::string PARAM_THICKNESS_GAUSS_SIGMA;
	static const std::string PARAM_PARTICLE_THICKNESS;
	static const std::string PARAM_THICKNESS_SIZE;

private: // auxiliary functions
	bool setupShaders();
	bool setupObjects();
	bool setupFramebuffers();
	bool setupTextures();
	void setupThicknessGauss();

protected: // rendering parameters
	std::set<std::string> _parameterNames;
	float _particleSize;
	float _particleThickness;
	float _thicknessGaussSigma;
	float _thicknessGaussSize;
	float _thicknessTextureSize;

protected: // graphic objects
	// FrameBuffers
	FrameBufferPtr _waterFB;
	FrameBufferPtr _waterThicknessFB;

	// textures
	TexturePtr _waterThicknessTexture;
	TexturePtr _thicknessAuxTexture;
	TexturePtr _waterLinDetphTexture;
	TexturePtr _waterDepthTexture;
	TexturePtr _gaussDist1DTexture;

	// objects
	GfxObjectPtr _water;

	// screenQuads
	ScreenQuadPtr _blurQuad;
	ScreenQuadPtr _finalQuad;
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
	ScreenSpaceParticleRenderer(AbstractScene* scene);
	virtual ~ScreenSpaceParticleRenderer(void);

	virtual bool setup();
	virtual void resize(int width, int height);
	virtual void render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData);
	virtual const std::set<std::string>& getParameters();
	virtual bool changeParameter(const std::string& parameter, ParamOperation operation);

	virtual void smoothWaterTexture();
};

