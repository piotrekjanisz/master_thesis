#pragma once

#include "Texture.h"
#include "AbstractScene.h"
#include <utils/ParameterController.h>

#include <vector>
#include <string>
#include <set>

struct ParticleData
{
	int particleCount;
	float* particles;
	float* particleDensity;
};

class ParticleRenderer : public Parametrized
{
private:
	AbstractScene* _scene;

public:
	ParticleRenderer(AbstractScene* scene) : _scene(scene){};
	virtual ~ParticleRenderer(void) {};

	virtual bool setup() = 0;
	virtual void resize(int width, int height) = 0;
	virtual void render(TexturePtr& sceneColorTexture, TexturePtr& sceneDepthTexture, ParticleData& particleData) = 0;

	AbstractScene* getScene() { return _scene; }
};

