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

	_sceneTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	CHECK_GL_CMD(_sceneTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_sceneTexture->load2DFloatDataNoMipMap(GL_RGBA8, 640, 480, 0, GL_RGBA, NULL));

	_sceneDepthTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	CHECK_GL_CMD(_sceneDepthTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_sceneDepthTexture->load2DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, 640, 480, 0, GL_DEPTH_COMPONENT, NULL));
	
	_screenQuadTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	CHECK_GL_CMD(_screenQuadTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_screenQuadTexture->load2DFloatDataNoMipMap(GL_RGBA8, 640, 480, 0, GL_RGBA, NULL));

	_depthTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	CHECK_GL_CMD(_depthTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_depthTexture->load2DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, 640, 480, 0, GL_DEPTH_COMPONENT, NULL));

	_smoothedTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	CHECK_GL_CMD(_smoothedTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_smoothedTexture->load2DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, 640, 480, 0, GL_DEPTH_COMPONENT, NULL));

	const int GAUSS_SIZE = 5;

	float* gaussData = new float[GAUSS_SIZE*GAUSS_SIZE];
	Filters::createGauss2D(GAUSS_SIZE, 1.0, 2.0, gaussData);
	_gaussDistTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	CHECK_GL_CMD(_gaussDistTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_gaussDistTexture->load2DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, GAUSS_SIZE, GAUSS_SIZE, 0, GL_DEPTH_COMPONENT, gaussData));

	Filters::createHeavisideDistribution(0.0, 1.0, 0.2, 25, gaussData);
	_spatialDistTexture = boost::make_shared<Texture>(GL_TEXTURE_1D);
	CHECK_GL_CMD(_spatialDistTexture->setParameters(GL_NEAREST, GL_CLAMP_TO_EDGE));
	CHECK_GL_CMD(_spatialDistTexture->load1DFloatDataNoMipMap(GL_DEPTH_COMPONENT32, 25, 0, GL_DEPTH_COMPONENT, gaussData));

	delete [] gaussData;	

	_sceneFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneTexture, GL_COLOR_ATTACHMENT0));
	CHECK_GL_CMD(_sceneFrameBuffer->attachTexture2D(_sceneDepthTexture, GL_DEPTH_ATTACHMENT));

	_waterFrameBuffer = boost::make_shared<FrameBuffer>();
	CHECK_GL_CMD(_waterFrameBuffer->attachRenderbuffer(GL_RGBA, GL_COLOR_ATTACHMENT0, 640, 480));
	CHECK_GL_CMD(_waterFrameBuffer->attachTexture2D(_depthTexture, GL_DEPTH_ATTACHMENT));

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
		CHECK_GL_CMD(_colorLocation = _shaderProgram->getUniformLocation("color"));
		CHECK_GL_CMD(_shaderProgram->bindFragDataLocation(0, "fragColor"));

		ShaderProgramPtr screenQuadShader = boost::make_shared<ShaderProgram>();
		//screenQuadShader->load("shaders/screen_quad_vertex.glsl", "shaders/draw_image2_fragment.glsl");
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
		CHECK_GL_CMD(_finalShader->bindFragDataLocation(0, "frag_color"));
		_finalQuad = boost::make_shared<ScreenQuad>(_finalShader);
	} catch (const BaseException& ex) {
		cout << ex.what() << endl;
		fgetc(stdin);
		return false;
	}
	CHECK_GL_CMD(_shaderProgram->useThis());

	try {
		_box = boost::make_shared<GfxStaticObject>(_shaderProgram);
		_plane = boost::make_shared<GfxStaticObject>(_shaderProgram);
		ShapePtr plane = ShapeFactory().createPlane(100.0f);
		ShapePtr box = ShapeFactory().createBox(0.5f);
		CHECK_GL_CMD(_box->createFromShape(box));
		CHECK_GL_CMD(_plane->createFromShape(plane));
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
	_shaderProgram->useThis();
	setupViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(_projectionLocation, 1, GL_FALSE, _projectionMatrix);
	glUniformMatrix4fv(_modelViewLocation, 1, GL_FALSE, _viewMatrix);
	glUniformMatrix3fv(_normalMatrixLocation, 1, GL_FALSE, getNormalMatrix(_viewMatrix));
	glUniform4fv(_colorLocation, 1, vmml::vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	_plane->render();
	
	glUniform4fv(_colorLocation, 1, vmml::vec4f(0.9f, 0.9f, 0.9f, 1.0f));
	int nbActors = physicsScene->getNbActors();
	NxActor** actors = physicsScene->getActors();

	// render boxes
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

	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _waterFrameBuffer->getId()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// render fluids
	NxFluid** fluids = physicsScene->getFluids();
	int nbFluids = physicsScene->getNbFluids();

	for(int i = 0; i < nbFluids; i++) {
		NxFluid* fluid = fluids[i];
		MyFluid* myFluid = (MyFluid*)fluid->userData;
		if (myFluid) {
			myFluid->render(_projectionMatrix, _viewMatrix);
		}
	}


	//_sceneFrameBuffer->copyRenderColorToScreen(GL_COLOR_ATTACHMENT0, 0, 0, getWidth(), getHeight(), 0.0 * getWidth(), 0.0 * getHeight(), getWidth(), getHeight());
	//_sceneFrameBuffer->copyRenderColorToScreen(GL_COLOR_ATTACHMENT0, 0, 0, getWidth(), getHeight(), 0.8 * getWidth(), 0.8 * getHeight(), getWidth(), getHeight());
	/*
	//FrameBuffer::bindDefaultForDrawing();
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_screenQuad->attachTexture(_depthTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_screenQuad->attachTexture(_gaussDistTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->attachTexture(_spatialDistTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 1.0f / getHeight()));
	CHECK_GL_CMD(_screenQuad->render());
	*/


	// ------- new code ------------
	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, _smoothFrameBuffer->getId()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_screenQuad->attachTexture(_depthTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_screenQuad->attachTexture(_gaussDistTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_screenQuad->attachTexture(_spatialDistTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_screenQuad->getShaderProgram()->setUniform2f("coordStep", 1.0f / getWidth(), 1.0f / getHeight()));
	CHECK_GL_CMD(_screenQuad->render());

	CHECK_GL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	CHECK_GL_CMD(_finalQuad->attachTexture(_smoothedTexture, GL_TEXTURE0));
	CHECK_GL_CMD(_finalQuad->attachTexture(_sceneDepthTexture, GL_TEXTURE1));
	CHECK_GL_CMD(_finalQuad->attachTexture(_sceneTexture, GL_TEXTURE2));
	CHECK_GL_CMD(_finalQuad->render());
}
