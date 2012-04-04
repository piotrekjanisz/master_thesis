#pragma once

#include <NxPhysics.h>
#include "abstractscene.h"
#include "ShaderProgram.h"
#include "data_types.h"
#include "FrameBuffer.h"
#include "GfxStaticObject.h"
#include "Texture.h"
#include "ParticleRenderer.h"


class Scene2 : public AbstractScene
{
private: // auxiliary parameters
	int _particleCount;
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
		_particleRenderer = renderer;
	}
};

