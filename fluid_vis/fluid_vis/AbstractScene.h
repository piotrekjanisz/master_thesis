#pragma once

#include <vmmlib\vmmlib.hpp>
#include <boost\timer.hpp>
#include "CameraFrame.h"
#include "SpaceObject.h"
#include "Texture.h"


class AbstractScene
{
	float _fov;
	float _zNear;
	float _zFar;
	float _aspect;
	int _width;
	int _height;

	float _xTranslation;
	float _yTranslation;
	float _zTranslation;

	float _xRotation;
	float _yRotation;

	CameraFrame _cameraFrame;

	boost::timer _timer;
	int _frameCount;
	
protected:
	vmml::mat4f _projectionMatrix;
	vmml::mat4f _inverseProjectionMatrix;
	vmml::mat4f _viewMatrix;
	vmml::mat4f _modelViewProjectionMatrix;
	
	void setProjectionMatrix(float fov, float aspect, float zNear, float zFar);
	void setProjectionMatrix(float aspect) { setProjectionMatrix(_fov, aspect, _zNear, _zFar); }

	void setOrthographicProjectionMatrix(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);

	void setupMatrixes();

	vmml::vec4f _lightDirection;
	vmml::vec4f _lightPosition;
	SpaceObject _lightController;

public:
	AbstractScene() 
		: _frameCount(0), 
		_lightDirection(-1.0, -1.0, 1.0, 0.0),
		_lightPosition(50.0f, 50.0f, -50.0f, 1.0f),
		_lightController(vmml::vec3f(50.0f, 50.0f, -50.0f)) {}
	virtual ~AbstractScene() {}
	
	virtual bool setup();
	virtual void render() = 0;
	virtual void reshape(int width, int height);
	virtual void translate(float x, float y, float z);
	virtual void rotateX(float val);
	virtual void rotateY(float val);

	virtual TexturePtr& getEnvironmentTexture() = 0;

	void setFov(float fov) { _fov = fov; }
	void setZNear(float zNear) { _zNear = zNear; }
	void setZFar(float zFar) { _zFar = zFar; }

	float getFov() const { return _fov; }
	float getZNear() const { return _zNear; }
	float getZFar() const { return _zFar; }

	float getCtgFovX() const;
	float getCtgFovY() const;

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }

	vmml::mat4f& getProjectionMatrix() { return _projectionMatrix; } 
	vmml::mat4f& getInverseProjectionMatrix() { return _inverseProjectionMatrix; }
	vmml::mat4f& getViewMatrix() { return _viewMatrix; }
	vmml::mat4f& getModelViewProjectionMatrix() { return _modelViewProjectionMatrix; }

	vmml::vec4f getLightInEyeSpace() { return _viewMatrix * _lightDirection; }
	vmml::vec4f getLightPositionInEyeSpace() { return _viewMatrix * _lightPosition; }

	CameraFrame& cameraFrame() { return _cameraFrame; }
	const CameraFrame& cameraFrame() const { return _cameraFrame; }

	void computeFrameRate();

	virtual void displayAdditionalStats() = 0;


	vmml::mat3f getNormalMatrix(const vmml::mat4f& modelView);
};

