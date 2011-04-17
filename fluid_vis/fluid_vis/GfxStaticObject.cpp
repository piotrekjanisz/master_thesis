#include "GfxStaticObject.h"
#include <GL/glew.h>


GfxStaticObject::GfxStaticObject(const boost::shared_ptr<ShaderProgram>& shaderProgram)
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

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	_vbos.push_back(vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * components * sizeof(float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(attribLocation, components, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribLocation);
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
	addAttribute("normal", shape->normals, shape->verticesCount, 3);
	unsigned int indicesVBO;
	glGenBuffers(1, &indicesVBO);
	_vbos.push_back(indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape->indicesCount * sizeof(int), shape->indices, GL_STATIC_DRAW);
	_numberIndices = shape->indicesCount;
}

void GfxStaticObject::render()
{
	glBindVertexArray(_vao);
	_shaderProgram->useThis();
	glDrawElements(GL_TRIANGLES, _numberIndices, GL_UNSIGNED_INT, 0);
}