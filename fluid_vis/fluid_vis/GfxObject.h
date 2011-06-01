#pragma once

#include <map>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include <GL/glew.h>
#include "ShaderProgram.h"
#include "GfxStaticObject.h"

enum AttributeType 
{
	STATIC_ATTR,
	DYNAMIC_ATTR
};

class GfxObject
{
	struct ShaderAttribute 
	{
		ShaderAttribute() :vbo(-1), components(0), type(STATIC_ATTR), location(-1) {}
		unsigned int vbo;
		int components;
		AttributeType type;
		int location;
	};

	unsigned int _vao;
	std::map<std::string, ShaderAttribute> _attributes;
	std::map<std::string, int> _uniforms;
	int _numberIndices;
	boost::shared_ptr<ShaderProgram> _shaderProgram;

public:
	GfxObject(const boost::shared_ptr<ShaderProgram>& shaderProgram);
	~GfxObject(void);

	void addAttribute(const std::string& name, float* data, int count, int components, AttributeType type) throw(GfxException);
	void updateAttribute(const std::string& name, float* data, int count) throw(GfxException);

	void addUniformMat4f(const std::string& name) throw(GfxException);
	void updateUniformMat4f(const std::string& name, float* data) throw(GfxException);

	void render(int count, GLenum objType);

	boost::shared_ptr<ShaderProgram> getShaderProgram() const { return _shaderProgram; }
};

