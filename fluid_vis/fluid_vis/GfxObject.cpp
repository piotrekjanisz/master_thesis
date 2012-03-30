#include "GfxObject.h"
#include "debug_utils.h"

using namespace std;

GfxObject::GfxObject(void)
{
}


GfxObject::~GfxObject(void)
{
	for (map<string, Attribute>::iterator it = _attributes.begin(); it != _attributes.end(); ++it) {
		if (it->second.vbo >= 0) {
			glDeleteBuffers(1, &(it->second.vbo));
		}
	}
	for (map<ShaderProgramPtr, unsigned int>::iterator it = _shaderVaos.begin(); it != _shaderVaos.end(); ++it) {
		glDeleteVertexArrays(1, &(it->second));
	}
}

void GfxObject::addAttribute(const std::string& name, float* data, int count, int components, AttributeType type)
{
	Attribute attribute;
	attribute.components = components;
	attribute.type = type;

	GLenum bufferType;
	switch (type) {
	case STATIC_ATTR:
		bufferType = GL_STATIC_DRAW; break;
	case DYNAMIC_ATTR:
		bufferType = GL_DYNAMIC_DRAW; break;
	default:
		bufferType = GL_STATIC_DRAW; break;
	}

	glGenBuffers(1, &attribute.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, attribute.vbo);
	glBufferData(GL_ARRAY_BUFFER, count * components * sizeof(float), data, bufferType);

	_attributes[name] = attribute;
}

void GfxObject::updateAttribute(const std::string& name, float* data, int count)
{
	if (_attributes.count(name) > 0) {
		Attribute& attribute = _attributes[name];
		glBindBuffer(GL_ARRAY_BUFFER, attribute.vbo);
		glBufferData(GL_ARRAY_BUFFER, count * attribute.components * sizeof(float), data, GL_DYNAMIC_DRAW);
	} else {
		std::cerr << "No attribute \"" + name + "\"" << std::endl;
	}
}

void GfxObject::addShader(ShaderProgramPtr& shader)
{
	if (_shaderVaos.count(shader) > 0) 
		return;
	
	unsigned int newVao;
	glGenVertexArrays(1, &newVao);
	glBindVertexArray(newVao);
	for (map<string, Attribute>::iterator it = _attributes.begin(); it != _attributes.end(); ++it) {
		try {
			int attribLocation = shader->getAttribLocation(it->first.c_str());
			glBindBuffer(GL_ARRAY_BUFFER, it->second.vbo);
			glVertexAttribPointer(attribLocation, it->second.components, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(attribLocation);
		} catch (ShaderException& ex) {
			DEBUG_COUT(<< ex.what() << std::endl);
		}
	}
	_shaderVaos[shader] = newVao;
}

void GfxObject::render(int count, GLenum primitiveType, ShaderProgramPtr& shader)
{
	addShader(shader);
	shader->useThis();
	glBindVertexArray(_shaderVaos[shader]);
	glDrawArrays(primitiveType, 0, count);
}

void GfxObject::renderElements(ShaderProgramPtr& shader, GLenum primitiveType, int count, unsigned int* indices)
{
	addShader(shader);
	shader->useThis();
	glBindVertexArray(_shaderVaos[shader]);
	glDrawElements(primitiveType, count, GL_UNSIGNED_INT, indices);
}