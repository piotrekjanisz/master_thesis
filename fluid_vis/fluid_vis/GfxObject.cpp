#include "GfxObject.h"
#include <GL/glew.h>

using namespace std;

GfxObject::GfxObject(const boost::shared_ptr<ShaderProgram>& shaderProgram)
	: _shaderProgram(shaderProgram), _vao(-1), _numberIndices(-1)
{
	glGenVertexArrays(1, &_vao);
}


GfxObject::~GfxObject(void)
{
	if (_vao != -1) {
		glDeleteVertexArrays(1, &_vao);
	}
	for (map<string, ShaderAttribute>::iterator it = _attributes.begin(); it != _attributes.end(); ++it) {
		if (it->second.vbo != -1) {
			glDeleteBuffers(1, &(it->second.vbo));
		}
	}
}

void GfxObject::addAttribute(const std::string& name, float* data, int count, int components, AttributeType type) throw(GfxException)
{	
	ShaderAttribute attribute;
	try {
		attribute.location = _shaderProgram->getAttribLocation(name.c_str());
	} catch (const ShaderException& ex) {
		throw GfxException(ex.what());
	}
	
	attribute.type = type;
	attribute.components = components;
	glGenBuffers(1, &(attribute.vbo));
	_attributes[name] = attribute;
	
	GLenum bufferType;
	switch (type) {
	case STATIC_ATTR:
		bufferType = GL_STATIC_DRAW; break;
	case DYNAMIC_ATTR:
		bufferType = GL_DYNAMIC_DRAW; break;
	}

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, attribute.vbo);
	glBufferData(GL_ARRAY_BUFFER, count * components * sizeof(float), data, bufferType);
	glVertexAttribPointer(attribute.location, components, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute.location);
}

void GfxObject::updateAttribute(const std::string& name, float* data, int count) throw(GfxException)
{
	if (_attributes.count(name) > 0) {
		ShaderAttribute attribute = _attributes[name];
		glBindBuffer(GL_ARRAY_BUFFER, attribute.vbo);
		glBufferData(GL_ARRAY_BUFFER, attribute.components * sizeof(float) * count, data, GL_DYNAMIC_DRAW);
	}
}

void GfxObject::render(int count, GLenum objType)
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	glDrawArrays(GL_POINTS, 0, count);
}

void GfxObject::addUniformMat4f(const std::string& name) throw(GfxException)
{
	try {
		int uniformLocation = _shaderProgram->getUniformLocation(name.c_str());
		_uniforms[name] = uniformLocation;
	} catch (ShaderException& ex) {
		throw GfxException(ex.what());
	}
}

void GfxObject::updateUniformMat4f(const std::string& name, float* data) throw(GfxException)
{
	_shaderProgram->useThis();
	glUniformMatrix4fv(_uniforms[name], 1, GL_FALSE, data);
}