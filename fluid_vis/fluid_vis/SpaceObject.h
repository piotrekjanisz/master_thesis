#pragma once

#include <vmmlib\vmmlib.hpp>

class SpaceObject
{
public:
	SpaceObject(const vmml::vec3f& position)
		: _position(position) {}

	~SpaceObject(void);

	void translate(const vmml::vec3f& translation)
	{
		_position += translation;
	}


	vmml::mat4f getModelMatrix()
	{
		vmml::mat4f result = vmml::mat4f::IDENTITY;
		result.set_column(3, vmml::vec4f(_position, 1.0));
		return result;
	}

	const vmml::vec3f& position() const { return _position; }

private:
	vmml::vec3f _position;
};

