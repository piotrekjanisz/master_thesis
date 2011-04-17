#include "CameraFrame.h"
#include <GL/glus.h>

void CameraFrame::moveForward(float val)
{
	_position += val * _forward;
}

void CameraFrame::moveUp(float val)
{
	_position += val * _up;
}

void CameraFrame::moveAside(float val)
{
	_position += val * _forward.cross(_up);
}

void CameraFrame::rotateHorizontally(float val)
{
	_forward = _forward.rotate(val, vmml::vec3f(0.0f, 1.0f, 0.0f));
	_up = _up.rotate(val, vmml::vec3f(0.0f, 1.0f, 0.0f));
}

void CameraFrame::rotateVertically(float val)
{
	vmml::vec3f _xAxis = _forward.cross(_up);
	_forward = _forward.rotate(val, _xAxis);
	_up = _up.rotate(val, _xAxis);
}

vmml::mat4f CameraFrame::getTransformation()
{
	vmml::mat4f retVal;
	vmml::vec3f center = _position + _forward;
	glusLookAtf(retVal, _position.x(), _position.y(), _position.z(), center.x(), center.y(), center.z(), _up.x(), _up.y(), _up.z());
	return retVal;
}
