#include "ShapeFactory.h"

#include <boost/make_shared.hpp>
#include <GL/glus.h>


ShapeFactory::ShapeFactory(void)
{
}


ShapeFactory::~ShapeFactory(void)
{
}

ShapePtr ShapeFactory::createPlane(float halfExtent)
{
	const unsigned int VERTICES_COUNT = 5 * 4;
	const unsigned int INDICES_COUNT = 4 * 3;
	float* vertices = new float[VERTICES_COUNT];
	float* normals = new float[VERTICES_COUNT];
	unsigned int* indices = new unsigned int[INDICES_COUNT];

	/*
	 *	0---1
	 *  |\ /|				  x
	 *  | 4 |           ------>
	 *  |/ \|           |
	 *  2---3           |
	 *                z \/
	 */ 

	int i = 0;
	vertices[i++] = -halfExtent; vertices[i++] = 0.0f; vertices[i++] = -halfExtent; vertices[i++] = 1.0f; // 0
	vertices[i++] =  halfExtent; vertices[i++] = 0.0f; vertices[i++] = -halfExtent; vertices[i++] = 1.0f; // 1
	vertices[i++] = -halfExtent; vertices[i++] = 0.0f; vertices[i++] =  halfExtent; vertices[i++] = 1.0f; // 2
	vertices[i++] =  halfExtent; vertices[i++] = 0.0f; vertices[i++] =  halfExtent; vertices[i++] = 1.0f; // 3
	vertices[i++] =  0.0f;       vertices[i++] = 0.0f; vertices[i++] =  0.0f;       vertices[i++] = 1.0f; // 4

	i = 0;
	normals[i++] = 0.0f; normals[i++] = 1.0f; normals[i++] = 0.0f;
	normals[i++] = 0.0f; normals[i++] = 1.0f; normals[i++] = 0.0f;
	normals[i++] = 0.0f; normals[i++] = 1.0f; normals[i++] = 0.0f;
	normals[i++] = 0.0f; normals[i++] = 1.0f; normals[i++] = 0.0f;
	normals[i++] = 0.0f; normals[i++] = 1.0f; normals[i++] = 0.0f;

	i = 0;
	indices[i++] = 4; indices[i++] = 1; indices[i++] = 0;
	indices[i++] = 4; indices[i++] = 0; indices[i++] = 2;
	indices[i++] = 4; indices[i++] = 2; indices[i++] = 3;
	indices[i++] = 4; indices[i++] = 3; indices[i++] = 1;

	return boost::make_shared<Shape>(vertices, VERTICES_COUNT, normals, indices, INDICES_COUNT);
}

ShapePtr ShapeFactory::createBox(float halfExtent)
{
	GLUSshape box;
	glusCreateCubef(&box, halfExtent);
	return boost::make_shared<Shape>(box.vertices, box.numberVertices, box.normals, box.indices, box.numberIndices);
}
