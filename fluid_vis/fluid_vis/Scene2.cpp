#include "Scene2.h"
#include "debug_utils.h"
#include "MyFluid.h"
#include "ParticleRenderer.h"

#include <iostream>

Scene2::Scene2()
	: AbstractScene(),
	_particlesToShutDown(0)
{
	// no OpenGL code here!!
}


Scene2::~Scene2(void)
{
}

const std::set<std::string>& Scene2::getParameters()
{
	return _parameters;
}

bool Scene2::changeParameter(const std::string& parameter, ParamOperation operation)
{
	return _particleRenderer->changeParameter(parameter, operation);
}

TexturePtr& Scene2::getEnvironmentTexture()
{
	return _skyBoxTexture;
}

void Scene2::reshape(int width, int height)
{
	AbstractScene::reshape(width, height);

	CHECK_GL_CMD(_sceneColorTexture->resize2D(width, height));
	CHECK_GL_CMD(_sceneDepthTexture->resize2D(width, height));
	
	CHECK_GL_CMD(_particleRenderer->resize(width, height));
	CHECK_GL_CMD(_sceneFrameBuffer->resize(width, height));
}

bool Scene2::setupTextures()
{
	CHECK_GL_CMD(_sceneColorTexture = Texture::create2DRGBTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_sceneDepthTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480));
	CHECK_GL_CMD(_skyBoxTexture = Texture::createCubeMap(GL_LINEAR, GL_CLAMP_TO_EDGE, "textures/sk3"));
	CHECK_GL_CMD(_floorTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_REPEAT, "textures/floor_bl.tga"));
	CHECK_GL_CMD(_floorNormalMapTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_REPEAT, "textures/floor_bl_normal.tga"));
	CHECK_GL_CMD(_boxTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_CLAMP_TO_EDGE, "textures/box.tga"));
	
	return true;
}

bool Scene2::setupFramebuffers()
{
	_sceneFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneColorTexture, GL_COLOR_ATTACHMENT0));
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneDepthTexture, GL_DEPTH_ATTACHMENT));
	return true;
}

bool Scene2::setupShaders()
{
	try {
		_sceneShader = boost::make_shared<ShaderProgram>();
		_sceneShader->load("shaders/vertex.glsl", "shaders/fragment.glsl");
		CHECK_GL_CMD(_sceneShader->bindFragDataLocation(0, "fragColor"));
		CHECK_GL_CMD(_sceneShader->setUniform1i("tex", 0));

		_normalMapShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_normalMapShader->load("shaders/normal_map_vertex.glsl", "shaders/normal_map_fragment.glsl"));
		CHECK_GL_CMD(_normalMapShader->setUniform1i("tex", 0));
		CHECK_GL_CMD(_normalMapShader->setUniform1i("normal_map", 1));
		_normalMapShader->printParameters();

		_skyBoxShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_skyBoxShader->load("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl"));
		CHECK_GL_CMD(_skyBoxShader->setUniform1i("cubeMap", 0));
		CHECK_GL_CMD(_skyBoxShader->bindFragDataLocation(0, "frag_color"));

		_simpleShader = boost::make_shared<ShaderProgram>();
		_simpleShader->load("shaders/light_source_vertex.glsl", "shaders/light_source_fragment.glsl");
		CHECK_GL_CMD(_simpleShader->bindFragDataLocation(0, "fragColor"));
	} catch (const BaseException& ex) {
		std::cout << ex.what() << std::endl;
		fgetc(stdin);
		return false;
	}

	return true;
}

bool Scene2::setupObjects()
{
	try {
		_box = boost::make_shared<GfxStaticObject>(_sceneShader);
		_plane = boost::make_shared<GfxStaticObject>(_normalMapShader);
		_skyBox = boost::make_shared<GfxStaticObject>(_skyBoxShader);
		_light = boost::make_shared<GfxStaticObject>(_simpleShader);
		ShapePtr plane = ShapeFactory().createPlane(100.0f, 200.0f, 200);
		ShapePtr box = ShapeFactory().createBox(0.5f);
		ShapePtr skyBox = ShapeFactory().createSkyBox(50.0f);
		GLUSshape sphere;
		glusCreateSpheref(&sphere, 1.0f, 32);
		CHECK_GL_CMD(_box->createFromShape(box));
		CHECK_GL_CMD(_plane->createFromShape(plane));
		CHECK_GL_CMD(_skyBox->createFromShape(skyBox));
		CHECK_GL_CMD(_light->createFromGlusShape(sphere));
	} catch (const BaseException& ex) {
		std::cout << ex.what() << std::endl;
		fgetc(stdin);
		return false;
	}

	return true;
}


bool Scene2::setup()
{
	if (!AbstractScene::setup()) {
		return false;
	}
	
	translate(-2.0f, 5.0f, -5.0f);
	rotateY(-0.5f);
	rotateX(-0.5f);

	if (! setupTextures())
		return false;

	if (! setupFramebuffers())
		return false;
	
	if (! setupShaders())
		return false;

	if (! setupObjects())
		return false;

	if (! _particleRenderer->setup())
		return false;

	glLineWidth(5.0f);

	return true;
}

void Scene2::render()
{
}

void Scene2::render(NxScene* physicsScene)
{
	setupMatrixes();

	vmml::vec4f lightDirectionEyeSpace = getLightInEyeSpace();
	vmml::vec3f normalMatrix = getNormalMatrix(_viewMatrix);
	lightDirectionEyeSpace.normalize();

	vmml::vec4f lightPositionEyeSpace = _viewMatrix * _lightController.getModelMatrix() * vmml::vec4f(0.0f, 0.0f, 0.0f, 1.0f);

	// render scene
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _sceneFrameBuffer->getId()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render skybox
	CHECK_GL_CMD(_skyBoxShader->useThis());
	CHECK_GL_CMD(_skyBoxShader->setUniformMat4f("projectionMatrix", _projectionMatrix));
	CHECK_GL_CMD(_skyBoxShader->setUniformMat4f("modelViewMatrix", _viewMatrix));
	CHECK_GL_CMD(_skyBoxTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_skyBox->render());

	vmml::vec4f lightColor(1.0f, 1.0f, 0.0f, 1.0f);
	CHECK_GL_CMD(_light->getShader()->useThis());
	CHECK_GL_CMD(_light->getShader()->setUniform4f("color", lightColor));
	CHECK_GL_CMD(_light->getShader()->setUniformMat4f("modelViewProjectionMatrix", _projectionMatrix * _viewMatrix * _lightController.getModelMatrix()));
	CHECK_GL_CMD(_light->render());

	CHECK_GL_CMD(_plane->getShader()->setUniformMat4f("modelViewMatrix", _viewMatrix));
	CHECK_GL_CMD(_plane->getShader()->setUniformMat4f("projectionMatrix", _projectionMatrix));
	CHECK_GL_CMD(_plane->getShader()->setUniformMat3f("normalMatrix", getNormalMatrix(_viewMatrix)));
	CHECK_GL_CMD(_plane->getShader()->setUniform4f("lightPositionEyeSpace", lightPositionEyeSpace));
	CHECK_GL_CMD(_floorTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_floorNormalMapTexture->bindToTextureUnit(GL_TEXTURE1));
	CHECK_GL_CMD(_plane->render());

	int nbActors = physicsScene->getNbActors();
	NxActor** actors = physicsScene->getActors();

	_sceneShader->useThis();
	CHECK_GL_CMD(_sceneShader->setUniformMat4f("projectionMatrix", _projectionMatrix));
	CHECK_GL_CMD(_sceneShader->setUniform4f("lightDirection", lightDirectionEyeSpace));
	// render boxes
	_boxTexture->bindToTextureUnit(GL_TEXTURE0);
	vmml::mat4f modelMatrix;
	vmml::mat4f modelViewMatrix;
	while(nbActors--) {
		NxActor* actor = *actors++;
		if (!actor->userData) 
			continue;

		actor->getGlobalPose().getColumnMajor44(modelMatrix);
		modelViewMatrix = _viewMatrix * modelMatrix;
		CHECK_GL_CMD(_box->getShader()->setUniformMat4f("modelViewMatrix", modelViewMatrix));
		CHECK_GL_CMD(_box->getShader()->setUniformMat3f("normalMatrix", getNormalMatrix(modelViewMatrix)));
		CHECK_GL_CMD(_box->render());
	}


	NxFluid** fluids = physicsScene->getFluids();
	NxFluid* fluid = fluids[0];
	MyFluid* myFluid = (MyFluid*)fluid->userData;
	if (myFluid) {
		ParticleData data;

		data.particleCount = myFluid->getParticlesCount();
		data.particles = myFluid->getPositionsAsync();
		data.particleDensity = myFluid->getDensityAsync();

		_particleRenderer->render(_sceneColorTexture, _sceneDepthTexture, data);

		_particleCount = data.particleCount;
	} else {
		std::cout << "WARNING: no fluid" << std::endl;
	}

	computeFrameRate();
}

void Scene2::displayAdditionalStats()
{
	std::cout << "PARTICLE COUNT: " << _particleCount << std::endl;
	if (_particlesToShutDown > 0 && _particleCount > _particlesToShutDown)
		exit(0);
}