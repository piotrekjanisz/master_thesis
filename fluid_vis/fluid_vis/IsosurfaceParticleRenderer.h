#pragma once

#include "ParticleRenderer.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "GfxObject.h"
#include "data_types.h"
#include <surface_extraction/data_types.h>
#include <surface_extraction\MtSurfaceExtractor.h>
#include <list>
#include <set>

class IsosurfaceParticleRenderer : public ParticleRenderer
{
private:
	MtSurfaceExtractorPtr _surfaceExtractor;
	std::list<TriangleMesh> _outputs[2];
	int _currentOutput;
	std::set<std::string> _parameters;

private: // graphic objects
	FrameBufferPtr _frameBuffer;

	ShaderProgramPtr _sceneShader;
	ShaderProgramPtr _waterShader;

	ScreenQuadPtr _screenQuad;

	GfxObjectPtr _isoWater;
public:
	IsosurfaceParticleRenderer(AbstractScene* scene);
	~IsosurfaceParticleRenderer(void);

	virtual bool setup();
	virtual void resize(int width, int height);
	virtual void render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData);

	virtual const std::set<std::string>& getParameters();
	virtual bool changeParameter(const std::string& parameter, ParamOperation operation);
};

