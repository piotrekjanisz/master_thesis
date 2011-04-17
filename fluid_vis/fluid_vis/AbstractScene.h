#pragma once

#include <vmmlib\vmmlib.hpp>
#include "CameraFrame.h"


class AbstractScene
{
	float _fov;
	float _zNear;
	float _zFar;

	float _xTranslation;
	float _yTranslation;
	float _zTranslation;

	float _xRotation;
	float _yRotation;

	CameraFrame _cameraFrame;
	
protected:
	vmml::mat4f _projectionMatrix;
	vmml::mat4f _viewMatrix;
	
	void setProjectionMatrix(float fov, float aspect, float zNear, float zFar);
	void setProjectionMatrix(float aspect) { setProjectionMatrix(_fov, aspect, _zNear, _zFar); }

	void setupViewMatrix();
	
	vmml::mat3f getNormalMatrix(const vmml::mat4f& modelView);

public:
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

	CameraFrame& cameraFrame() { return _cameraFrame; }
	const CameraFrame& cameraFrame() const { return _cameraFrame; }
};

