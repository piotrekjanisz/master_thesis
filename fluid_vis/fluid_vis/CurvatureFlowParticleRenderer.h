#pragma once

#include "ParticleRenderer.h"
#include "FrameBuffer.h"
#include "GfxObject.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "data_types.h"

class CurvatureFlowParticleRenderer : public ParticleRenderer
{
private:
	bool setupShaders();
	bool setupObjects();
	bool setupFramebuffers();
	bool setupTextures();

private: // rendering parameters
	float _particleSize;
	float _particleDepth;
	float _edgeTreshold;
	float _timeStep;
	int _blurIterationCount;

	float _thicknessGaussSigma;
	int _thicknessGaussSize;

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

	// objects
	GfxObjectPtr _water;

	// screenQuads
	ScreenQuadPtr _blurQuad;
	ScreenQuadPtr _finalQuad;
	ScreenQuadPtr _edgeQuad;
	ScreenQuadPtr _curvatureFlowQuad;

	// shaders
	ShaderProgramPtr _waterThicknessShader;
	ShaderProgramPtr _waterShader;
	ShaderProgramPtr _gaussianBlurShader;
	ShaderProgramPtr _finalShader;
	ShaderProgramPtr _edgeDetectionShader;
	ShaderProgramPtr _curvatureFlowShader;
public:
	CurvatureFlowParticleRenderer(AbstractScene* scene);
	virtual ~CurvatureFlowParticleRenderer(void);

	bool setup();
	virtual void resize(int width, int height);
	virtual void render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData);
};

