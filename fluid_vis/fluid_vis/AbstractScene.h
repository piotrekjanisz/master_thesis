#pragma once

#include <vmmlib\vmmlib.hpp>
#include <boost\timer.hpp>
#include "CameraFrame.h"


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
	
	void setProjectionMatrix(float fov, float aspect, float zNear, float zFar);
	void setProjectionMatrix(float aspect) { setProjectionMatrix(_fov, aspect, _zNear, _zFar); }

	void setOrthographicProjectionMatrix(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);

	void setupViewMatrix();
	
	vmml::mat3f getNormalMatrix(const vmml::mat4f& modelView);

public:
	AbstractScene() : _frameCount(0) {}
	virtual ~AbstractScene() {}
	
	virtual bool setup();
	virtual void render() = 0;
	virtual void reshape(int width, int height);
	virtual void translate(float x, float y, float z);
	virtual void rotateX(float val);
	virtual void rotateY(float val);

	void setFov(float fov) { _fov = fov; }
	void setZNear(float zNear) { _zNear = zNear; }
	void setZFar(float zFar) { _zFar = zFar; }

	float getFov() const { return _fov; }
	float getZNear() const { return _zNear; }
	float getZFar() const { return _zFar; }

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }

	CameraFrame& cameraFrame() { return _cameraFrame; }
	const CameraFrame& cameraFrame() const { return _cameraFrame; }

	void computeFrameRate();
};

