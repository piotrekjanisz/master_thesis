#define DEBUG

#include "Scene.h"
#include "debug_utils.h"
#include "filters.h"
#include <GL/glus.h>
#include <GL/glew.h>
#include <iostream>
#include <boost/make_shared.hpp>

using namespace std;

Scene::Scene()
{
	// don't put any opengl command here!!!
}

bool Scene::setup()
{
	if (!AbstractScene::setup()) {
		return false;
	}

	DEBUG_CODE(_debugData = new float[640*480]);

	// Textures
	_sceneTexture = Texture::create2DRGBTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480);
	_sceneDepthTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480);
	//_zTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, 0);
	CHECK_GL_CMD(_zTexture = Texture::create2DTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480, GL_R32F, GL_RED));
	_screenQuadTexture = Texture::create2DRGBTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480);
	_depthTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480);
	_smoothedTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 640, 480);
	_floorTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_REPEAT, "textures/floor.tga");
	_boxTexture = Texture::createTexture2DFromImage(GL_LINEAR, GL_CLAMP_TO_EDGE, "textures/box.tga");
	CHECK_GL_CMD(_skyBoxTexture = Texture::createCubeMap(GL_LINEAR, GL_CLAMP_TO_EDGE, "textures/sk"));

	// filters
	const int GAUSS_SIZE = 21;
	float* gaussData = new float[GAUSS_SIZE*GAUSS_SIZE];
	Filters::createGauss2D(GAUSS_SIZE, 1.0, 10.0, gaussData);
	_gaussDistTexture = Texture::create2DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, GAUSS_SIZE, GAUSS_SIZE, 0, gaussData);
	Filters::createHeavisideDistribution(0.0, 1.0, 0.1, 25, gaussData);
	_spatialDistTexture = Texture::create1DDepthTexture(GL_LINEAR, GL_CLAMP_TO_EDGE, 25, 0, gaussData);
	delete [] gaussData;	
	
	// framebuffers
	_sceneFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneTexture, GL_COLOR_ATTACHMENT0));
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneDepthTexture, GL_DEPTH_ATTACHMENT));

	_waterFrameBuffer = boost::make_shared<FrameBuffer>();
	//CHECK_GL_CMD(_waterFrameBuffer->attachRenderbuffer(GL_RGBA, GL_COLOR_ATTACHMENT0, 640, 480));
	CHECK_GL_CMD(_waterFrameBuffer->attachTexture2D(_depthTexture, GL_DEPTH_ATTACHMENT));
	CHECK_GL_CMD(_waterFrameBuffer->attachTexture2D(_zTexture, GL_COLOR_ATTACHMENT0));

	_smoothFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_smoothFrameBuffer->attachRenderbuffer(GL_RGBA, GL_COLOR_ATTACHMENT0, 640, 480));
	CHECK_GL_CMD(_smoothFrameBuffer->attachTexture2D(_smoothedTexture, GL_DEPTH_ATTACHMENT));

	translate(0.0f, 1.5f, -0.5f);
	rotateX(-0.4f);

	translate(0.0f, 0.0f, -5.0f);

	try {
		_shaderProgram = boost::make_shared<ShaderProgram>();
		_shaderProgram->load("shaders/vertex.glsl", "shaders/fragment.glsl");
		CHECK_GL_CMD(_projectionLocation = _shaderProgram->getUniformLocation("projectionMatrix"));
		CHECK_GL_CMD(_modelViewLocation = _shaderProgram->getUniformLocation("modelViewMatrix"));
		CHECK_GL_CMD(_normalMatrixLocation = _shaderProgram->getUniformLocation("normalMatrix"));
		CHECK_GL_CMD(_shaderProgram->bindFragDataLocation(0, "fragColor"));
		CHECK_GL_CMD(_shaderProgram->setUniform1i("tex", 0));

		ShaderProgramPtr screenQuadShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(screenQuadShader->load("shaders/screen_quad_vertex.glsl", "shaders/bilateral_gauss_fragment.glsl"));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("inputImage", 0));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("gaussianDist", 1));
		CHECK_GL_CMD(screenQuadShader->setUniform1i("spatialDist", 2));
		CHECK_GL_CMD(screenQuadShader->bindFragDataLocation(0, "frag_color"));
		_screenQuad = boost::make_shared<ScreenQuad>(screenQuadShader);

		_finalShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_finalShader->load("shaders/screen_quad_vertex.glsl", "shaders/final_stage_fragment.glsl"));
		CHECK_GL_CMD(_finalShader->setUniform1i("depthTexture", 0));
		CHECK_GL_CMD(_finalShader->setUniform1i("sceneDepthTexture", 1));
		CHECK_GL_CMD(_finalShader->setUniform1i("sceneTexture", 2));
		CHECK_GL_CMD(_inverseProjectionLocation = _finalShader->getUniformLocation("inverseProjection"));
		CHECK_GL_CMD(_finalShader->bindFragDataLocation(0, "frag_color"));
		_finalQuad = boost::make_shared<ScreenQuad>(_finalShader);

		_skyBoxShader = boost::make_shared<ShaderProgram>();
		CHECK_GL_CMD(_skyBoxShader->load("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl"));
		CHECK_GL_CMD(_skyBoxShader->setUniform1i("cubeMap", 0));
		CHECK_GL_CMD(_skyBoxShader->bindFragDataLocation(0, "frag_color"));
		CHECK_GL_CMD(_skyBoxProjectionLocation = _skyBoxShader->getUniformLocation("projectionMatrix"));
		CHECK_GL_CMD(_skyBoxModelViewLocation = _skyBoxShader->getUniformLocation("modelViewMatrix"));
	} catch (const BaseException& ex) {
		cout << ex.what() << endl;
		fgetc(stdin);
		return false;
	}
	CHECK_GL_CMD(_shaderProgram->useThis());

	try {
		_box = boost::make_shared<GfxStaticObject>(_shaderProgram);
		_plane = boost::make_shared<GfxStaticObject>(_shaderProgram);
		_skyBox = boost::make_shared<GfxStaticObject>(_skyBoxShader);
		ShapePtr plane = ShapeFactory().createPlane(100.0f, 10.0);
		ShapePtr box = ShapeFactory().createBox(0.5f);
		ShapePtr skyBox = ShapeFactory().createSkyBox(20.0f);
		CHECK_GL_CMD(_box->createFromShape(box));
		CHECK_GL_CMD(_plane->createFromShape(plane));
		CHECK_GL_CMD(_skyBox->createFromShape(skyBox));
	} catch(const GfxException& ex) {
		cout << ex.what() << endl;
		fgetc(stdin);
		return false;
	}

	return true;
}

void Scene::render()
{
	_shaderProgram->useThis();
	setupViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(_projectionLocation, 1, GL_FALSE, _projectionMatrix);
	glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, _viewMatrix);
	glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(_viewMatrix));
	glUniform4fv(_colorLocation, 1, vmml::vec4f(0.9f, 0.9f, 0.9f, 1.0f));
	_box->render();
	glUniform4fv(_colorLocation, 1, vmml::vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	_plane->render();
}

GLenum windowBuffs[] = {GL_BACK_LEFT};
GLenum fboBuffs[] = {GL_COLOR_ATTACHMENT0};

void Scene::render(NxScene* physicsScene)
{
	// render scene
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _sceneFrameBuffer->getId()));
	
	/*
	// render skybox
	CHECK_GL_CMD(_skyBoxShader->useThis());
	CHECK_GL_CMD(glUniformMatrix4fv(_skyBoxProjectionLocation, 1, GL_FALSE, _projectionMatrix));
	CHECK_GL_CMD(glUniformMatrix4fv(_skyBoxModelViewLocation, 1, GL_FALSE, _viewMatrix));
	CHECK_GL_CMD(_skyBoxTexture->bindToTextureUnit(GL_TEXTURE0));
	CHECK_GL_CMD(_skyBox->renderArrays(GL_QUADS));
	*/

	_shaderProgram->useThis();
	setupViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(_projectionLocation, 1, GL_FALSE, _projectionMatrix);
	glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, _viewMatrix);
	glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(_viewMatrix));
	//glUniform4fv(_colorLocation, 1, vmml::vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	_floorTexture->bindToTextureUnit(GL_TEXTURE0);
	_plane->render();

	int nbActors = physicsScene->getNbActors();
	NxActor** actors = physicsScene->getActors();

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
		glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, modelViewMatrix);
		glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(modelViewMatrix));
		_box->render();
	}

	// render fluids into depth buffer
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _waterFrameBuffer->getId()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	NxFluid** fluids = physicsScene->getFluids();
	int nbFluids = physicsScene->getNbFluids();

	for(int i = 0; i < nbFluids; i++) {
		NxFluid* fluid = fluids[i];
		MyFluid* myFluid = (MyFluid*)fluid->userData;
		if (myFluid) {
			CHECK_GL_CMD(myFluid->render(_projectionMatrix, _viewMatrix));
		}
	}

	CHECK_GL_CMD(_zTexture->getData(0, GL_RED, GL_FLOAT, _debugData));
	DebugUtils::printArray(_debugData, 1, 5);

	// smooth water depth
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _smoothFrameBuffer->getId()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_screenQuad->attachTexture(_depthTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_screenQuad->attachTexture(_gaussDistTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->attachTexture(_spatialDistTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 1.0f / getHeight()));
	CHECK_GL_CMD(_screenQuad->render());

	// put water together with rest of the scene
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_finalQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_finalQuad->attachTexture(_sceneDepthTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_finalQuad->attachTexture(_sceneTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 1.0f / getHeight()));
	CHECK_GL_CMD(_finalQuad->getShaderProgram()->useThis());
	CHECK_GL_CMD(glUniformMatrix4fv(_inverseProjectionLocation, 1, GL_FALSE, _inverseProjectionMatrix));
	CHECK_GL_CMD(_finalQuad->render());

}
