#pragma once

#include <utils/ParameterController.h>
#include "ScreenSpaceParticleRenderer.h"
#include "FrameBuffer.h"
#include "GfxObject.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "data_types.h"


class CurvatureFlowParticleRenderer : public ScreenSpaceParticleRenderer
{
private: // constants
	static const std::string PARAM_BLUR_ITERATION_COUNT;
	static const std::string PARAM_TIME_STEP;
	static const std::string PARAM_EDGE_TRESHOLD;

private: // auxiliary functions
	bool setupShaders();
	bool setupObjects();
	bool setupFramebuffers();
	bool setupTextures();

private: // rendering parameters
	float _edgeTreshold;
	float _timeStep;
	int _blurIterationCount;

private: // graphic objects
	// framebuffers
	FrameBufferPtr _smoothFB;

	// textures
	TexturePtr _smoothedTexture;
	TexturePtr _edgeTexture;

	// objects
	GfxObjectPtr _water;

	// screenQuads
	ScreenQuadPtr _edgeQuad;
	ScreenQuadPtr _curvatureFlowQuad;

	// shaders
	ShaderProgramPtr _edgeDetectionShader;
	ShaderProgramPtr _curvatureFlowShader;
public:
	CurvatureFlowParticleRenderer(AbstractScene* scene);
	virtual ~CurvatureFlowParticleRenderer(void);

	virtual bool setup();
	virtual void resize(int width, int height);
	virtual const std::set<std::string>& getParameters();
	virtual bool changeParameter(const std::string& parameter, ParamOperation operation);

	virtual void smoothWaterTexture();
};

