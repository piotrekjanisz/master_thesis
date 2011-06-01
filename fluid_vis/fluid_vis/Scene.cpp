#include "Scene.h"
#include "debug_utils.h"
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
	
	_screenQuadTexture = boost::make_shared<Texture>(GL_TEXTURE_2D);
	_screenQuadTexture->setParameters(GL_LINEAR, GL_CLAMP_TO_EDGE);
	_screenQuadTexture->load2DFloatDataNoMipMap(GL_RGBA8, 640, 480, 0, GL_RGBA, NULL);

	DEBUG_CODE(_debugData = new float[_screenQuadTexture->getWidth() * _screenQuadTexture->getHeight() * 4]);

	
	//_sceneFrameBuffer->attachRenderbuffer(GL_RGBA8, GL_COLOR_ATTACHMENT0, 1366, 768);

	_sceneFrameBuffer = boost::make_shared<FrameBuffer>();
	_sceneFrameBuffer->attachRenderbuffer(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, 1366, 768);
	_sceneFrameBuffer->attachTexture2D(_screenQuadTexture, GL_COLOR_ATTACHMENT0);
	
	_shaderProgram = boost::make_shared<ShaderProgram>();
	_box = boost::make_shared<GfxStaticObject>(_shaderProgram);
	_plane = boost::make_shared<GfxStaticObject>(_shaderProgram);

	translate(0.0f, 1.5f, -0.5f);
	rotateX(-0.4f);

	translate(0.0f, 0.0f, -5.0f);

	try {
		_shaderProgram->load("shaders/vertex.glsl", "shaders/fragment.glsl");
		_projectionLocation = _shaderProgram->getUniformLocation("projectionMatrix");
		_modelViewLocation = _shaderProgram->getUniformLocation("modelViewMatrix");
		_normalMatrixLocation = _shaderProgram->getUniformLocation("normalMatrix");
		_colorLocation = _shaderProgram->getUniformLocation("color");
		_shaderProgram->bindFragDataLocation(0, "fragColor");

		ShaderProgramPtr screenQuadShader = boost::make_shared<ShaderProgram>();
		screenQuadShader->load("shaders/screen_quad_vertex.glsl", "shaders/draw_image2_fragment.glsl");
		screenQuadShader->setUniform1i("inputImage", 0);
		_screenQuad = boost::make_shared<ScreenQuad>(screenQuadShader);
	} catch (const BaseException& ex) {
		cout << ex.what() << endl;
		fgetc(stdin);
		return false;
	}
	_shaderProgram->useThis();

	try {
		ShapePtr plane = ShapeFactory().createPlane(100.0f);
		ShapePtr box = ShapeFactory().createBox(0.5f);
		_box->createFromShape(box);
		_plane->createFromShape(plane);
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
	// FrameBuffer::bindDefaultForDrawing();
	//_sceneFrameBuffer->bindForDrawing();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glDrawBuffers(1, fboBuffs);
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

	glBindFramebuffer(GL_FRAMEBUFFER, _sceneFrameBuffer->getId());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// render fluids
	NxFluid** fluids = physicsScene->getFluids();
	int nbFluids = physicsScene->getNbFluids();

	for(int i = 0; i < nbFluids; i++) {
		NxFluid* fluid = fluids[i];
		MyFluid* myFluid = (MyFluid*)fluid->userData;
		if (myFluid) {
			// TODO move fluid rendering code here
			myFluid->render(_projectionMatrix, _viewMatrix);
		}
	}

	
	//_sceneFrameBuffer->copyRenderColorToScreen(GL_COLOR_ATTACHMENT0, 0, 0, getWidth(), getHeight(), 0.0 * getWidth(), 0.0 * getHeight(), getWidth(), getHeight());
	//_sceneFrameBuffer->copyRenderColorToScreen(GL_COLOR_ATTACHMENT0, 0, 0, getWidth(), getHeight(), 0.8 * getWidth(), 0.8 * getHeight(), getWidth(), getHeight());
	
	//FrameBuffer::bindDefaultForDrawing();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glDrawBuffers(1, windowBuffs);
	_screenQuad->attachTexture(_screenQuadTexture, GL_TEXTURE0);
	_screenQuad->render();

	//DEBUG_CODE(_screenQuadTexture->getData(0, GL_RGBA, GL_FLOAT, _debugData));
	//DEBUG_CODE(DebugUtils::printArray(_debugData, 100, 4));
	
	//_sceneFrameBuffer->copyRenderColorToScreen(GL_COLOR_ATTACHMENT0, 0, 0, getWidth(), getHeight(), 0.0 * getWidth(), 0.0 * getHeight(), getWidth(), getHeight());
	//_sceneFrameBuffer->copyRenderColorToScreen(GL_COLOR_ATTACHMENT0, 0, 0, getWidth(), getHeight(), 0.8 * getWidth(), 0.8 * getHeight(), getWidth(), getHeight());
	
	
}
