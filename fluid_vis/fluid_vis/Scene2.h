#pragma once

#include <NxPhysics.h>
#include <set>
#include "abstractscene.h"
#include "ShaderProgram.h"
#include "data_types.h"
#include "FrameBuffer.h"
#include "GfxStaticObject.h"
#include "Texture.h"
#include "ParticleRenderer.h"


class Scene2 : public AbstractScene, Parametrized
{
private: // auxiliary parameters
	int _particleCount;
	std::set<std::string> _parameters;

private: // rendering parameters
	ParticleRendererPtr _particleRenderer;

private: // graphic objects
	// framebuffers
	FrameBufferPtr _sceneFrameBuffer;

	// shaders
	ShaderProgramPtr _skyBoxShader;
	ShaderProgramPtr _sceneShader;
	ShaderProgramPtr _normalMapShader;
	ShaderProgramPtr _simpleShader;

	// textures
	TexturePtr _skyBoxTexture;
	TexturePtr _floorTexture;
	TexturePtr _floorNormalMapTexture;
	TexturePtr _boxTexture;
	TexturePtr _sceneColorTexture;
	TexturePtr _sceneDepthTexture;

	// objects
	GfxStaticObjectPtr _skyBox;
	GfxStaticObjectPtr _light;
	GfxStaticObjectPtr _plane;
	GfxStaticObjectPtr _box;

private: // auxiliary methods
	bool setupTextures();
	bool setupFramebuffers();
	bool setupShaders();
	bool setupObjects();

public:
	Scene2();
	~Scene2(void);

	virtual bool setup();
	virtual void render();
	virtual void reshape(int width, int height);

	void render(NxScene* physicsScene);

	virtual void displayAdditionalStats();

public: // setters and getters
	void setParticleRenderer(ParticleRendererPtr renderer)
	{
		if (_particleRenderer) {
			for (std::set<std::string>::iterator it = _particleRenderer->getParameters().begin(); it != _particleRenderer->getParameters().end(); ++it) {
				_parameters.erase(*it);
			}
		}

		_particleRenderer = renderer;
		_parameters.insert(renderer->getParameters().begin(), renderer->getParameters().end());
	}

	const std::set<std::string>& getParameters();

	bool changeParameter(const std::string& parameter, ParamOperation operation);
};

