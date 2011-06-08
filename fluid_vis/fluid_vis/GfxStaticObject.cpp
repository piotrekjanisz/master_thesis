#include "GfxStaticObject.h"
#include "debug_utils.h"
#include <GL/glew.h>


GfxStaticObject::GfxStaticObject(const ShaderProgramPtr& shaderProgram)
	: _shaderProgram(shaderProgram), _numberIndices(0)
{
	glGenVertexArrays(1, &_vao);
}

GfxStaticObject::~GfxStaticObject(void)
{
	if (_vao != -1)
		glDeleteVertexArrays(1, &_vao);
	for (std::vector<unsigned int>::iterator it = _vbos.begin(); it != _vbos.end(); ++it) {
		glDeleteBuffers(1, &(*it));
	}
}

void GfxStaticObject::addAttribute(const std::string& name, float* data, int count, int components) throw(GfxException)
{
	int attribLocation;
	try {
		attribLocation = _shaderProgram->getAttribLocation(name.c_str());
	} catch (const ShaderException& ex) {
		throw GfxException(ex.what());
	}

	glBindVertexArray(_vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	_vbos.push_back(vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * components * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(attribLocation, components, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribLocation);
}

void GfxStaticObject::addTexture(const std::string& name, int textureUnit) throw (GfxException)
{
	try {
		int samplerLocation = _shaderProgram->getUniformLocation(name.c_str());
		_shaderProgram->useThis();
		glUniform1i(samplerLocation, textureUnit);
	} catch (ShaderException& ex) {
		throw (GfxException(ex.what()));
	}
}

void GfxStaticObject::createFromGlusShape(const GLUSshape& shape) throw(GfxException)
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	addAttribute("vertex", shape.vertices, shape.numberVertices, 4);
	addAttribute("normal", shape.normals, shape.numberVertices, 3);
	unsigned int indicesVBO;
	glGenBuffers(1, &indicesVBO);
	_vbos.push_back(indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.numberIndices * sizeof(int), shape.indices, GL_STATIC_DRAW);
	_numberIndices = shape.numberIndices;
}

void GfxStaticObject::createFromShape(const ShapePtr& shape) throw(GfxException)
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	addAttribute("vertex", shape->vertices, shape->verticesCount, 4);
	if (shape->normals != 0) {
		addAttribute("normal", shape->normals, shape->verticesCount, 3);
	}	
	if (shape->texCoords != 0) {
		addAttribute("tex_coord", shape->texCoords, shape->verticesCount, 2);
	}
	unsigned int indicesVBO;
	glGenBuffers(1, &indicesVBO);
	_vbos.push_back(indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape->indicesCount * sizeof(int), shape->indices, GL_STATIC_DRAW);
	_numberIndices = shape->indicesCount;

	switch(shape->primitiveType) {
	case Shape::TRIANGLES:
		_primitiveType = GL_TRIANGLES;
		break;
	case Shape::QUADS:
		_primitiveType = GL_QUADS;
		break;
	default:
		_primitiveType = GL_TRIANGLES;
	}
}

void GfxStaticObject::createFromScreenQuad(const ShapePtr& quad) throw(GfxException)
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	addAttribute("vertex", quad->vertices, quad->verticesCount, 4);
	_numberVertices = quad->verticesCount;
}

void GfxStaticObject::render()
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	CHECK_GL_CMD(glDrawElements(_primitiveType, _numberIndices, GL_UNSIGNED_INT, 0));
}

void GfxStaticObject::renderArrays(GLenum mode) 
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	glDrawArrays(mode, 0, _numberVertices);
}