#pragma once

#include <string>
#include <list>
#include <map>
#include <GL/glew.h>
#include "ShaderProgram.h"
#include "data_types.h"
#include "NonCopyable.h"

class GfxObject : public NonCopyable
{
public:
	enum AttributeType 
	{
		STATIC_ATTR,
		DYNAMIC_ATTR
	};

private:
	struct Attribute
	{
		Attribute() :vbo(-1), components(0), type(STATIC_ATTR) {}
		unsigned int vbo;
		int components;
		AttributeType type;
	};

	std::map<std::string, Attribute> _attributes;
	std::map<ShaderProgramPtr, unsigned int> _shaderVaos;

public:
	GfxObject(void);
	~GfxObject(void);

	void addAttribute(const std::string& name, float* data, int count, int components, AttributeType type);
	void updateAttribute(const std::string& name, float* data, int count);
	void addShader(ShaderProgramPtr& shader);
	void render(int count, GLenum primitiveType, ShaderProgramPtr& shader);
};

