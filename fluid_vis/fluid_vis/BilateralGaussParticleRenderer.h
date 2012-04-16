#pragma once
#include "screenspaceparticlerenderer.h"

class BilateralGaussParticleRenderer :public ScreenSpaceParticleRenderer
{
private: // parameters
	int _maxFilter;
	float _edgeTreshold;
	float _bilateralGaussSigma;
	float _filterSizeMult;

private: // graphic objects
	FrameBufferPtr _frameBuffer;	
	ScreenQuadPtr _screenQuad;

	ShaderProgramPtr _bilateralGaussSmoothShader;

	TexturePtr _gaussDistributionsArrayTexture;
	TexturePtr _spatialDistTexture;
	TexturePtr _smoothAuxTexture;

public:
	BilateralGaussParticleRenderer(AbstractScene* scene);
	virtual ~BilateralGaussParticleRenderer(void);

	virtual void smoothWaterTexture();

	virtual bool setup();

	virtual void resize(int width, int height);
};

