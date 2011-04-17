#pragma once

#include <vmmlib\vmmlib.hpp>


class CameraFrame
{
	vmml::vec3f _forward;
	vmml::vec3f _up;
	vmml::vec3f _position;
public:
	CameraFrame()
		: _forward(vmml::vec3f(0.0f, 0.0f, -1.0f)), _up(vmml::vec3f(0.0f, 1.0f, 0.0f)) {}

	CameraFrame(const vmml::vec3f& forward, const vmml::vec3f& up, const vmml::vec3f& position)
		: _forward(forward), _up(up), _position(position) {}

	void moveForward(float val);
	void moveUp(float val);
	void moveAside(float val);

	void rotateHorizontally(float val);
	void rotateVertically(float val);

	vmml::mat4f getTransformation();

	~CameraFrame(void) {}

	vmml::vec3f& up() { return _up; }
	const vmml::vec3f& up() const { return _up; }

	vmml::vec3f& forward() { return _forward; }
	const vmml::vec3f& forward() const { return _forward; }

	vmml::vec3f& position() { return _position; }
	const vmml::vec3f& position() const { return _position; }

	vmml::vec3f xAxis() { return _forward.cross(_up); }
};

