#include "AbstractScene.h"
#include <GL/glew.h>

const float PI = 3.14159265358979323846;

bool AbstractScene::setup()
{
	_xTranslation = _yTranslation = _zTranslation = 0.0f;
	_xRotation = _yRotation = 0.0f;
	_zNear = 1.0f;
	_zFar = 1000.0f;
	_fov = 40.0f;

	setProjectionMatrix(1.3333f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	
	return true;
}

void AbstractScene::setProjectionMatrix(float fov, float aspect, float zNear, float zFar)
{
	float xymax = zNear * tan(fov * PI / 360.f);
	float ymin = -xymax;
	float xmin = -xymax;

	float width = xymax - xmin;
	float height = xymax - ymin;

	float depth = zFar - zNear;
	float q = -(zFar + zNear) / depth;
	float qn = -2 * (zFar * zNear) / depth;

	float w = 2 * zNear / width;
	w = w / aspect;
	float h = 2 * zNear / height;

	_projectionMatrix.set_column(0, vmml::vec4f(w, 0.0f, 0.0f, 0.0f));
	_projectionMatrix.set_column(1, vmml::vec4f(0.0f, h, 0.0f, 0.0f));
	_projectionMatrix.set_column(2, vmml::vec4f(0.0f, 0.0f, q, -1.0f));
	_projectionMatrix.set_column(3, vmml::vec4f(0.0f, 0.0f, qn, 0.0f));
}


void AbstractScene::reshape(int width, int height)
{
	setProjectionMatrix(width/(float)height);
}

void AbstractScene::translate(float x, float y, float z)
{
	_xTranslation += x;
	_yTranslation += y;
	_zTranslation += z;

	_cameraFrame.moveForward(z);
	_cameraFrame.moveUp(y);
	_cameraFrame.moveAside(x);
}

vmml::mat3f AbstractScene::getNormalMatrix(const vmml::mat4f& modelView)
{
	vmml::mat3f retVal;
	modelView.get_sub_matrix(retVal);
	return retVal;
}

void AbstractScene::setupViewMatrix()
{
	_viewMatrix = _cameraFrame.getTransformation();
	/*
	_viewMatrix = vmml::mat4f::IDENTITY;
	_viewMatrix.set_translation(_xTranslation, _yTranslation, _zTranslation);
	_viewMatrix.rotate_x(_xRotation);
	_viewMatrix.rotate_y(_yRotation);
	*/
}

void AbstractScene::rotateX(float val)
{ 
	_xRotation += val; 
	_cameraFrame.rotateVertically(val);
}

void AbstractScene::rotateY(float val)
{
	_yRotation += val;
	_cameraFrame.rotateHorizontally(val);
}