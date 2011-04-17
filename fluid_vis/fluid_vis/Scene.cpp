#include "Scene.h"
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

void Scene::render(NxScene* physicsScene)
{
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
}
