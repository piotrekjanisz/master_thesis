#pragma once

#include <GL/glus.h>
#include <boost/smart_ptr.hpp>

struct Shape
{
	unsigned int* indices;
	float* vertices;
	float* normals;
	unsigned int indicesCount;
	unsigned int verticesCount;

	Shape(float* _vertices, unsigned int _verticesCount, float* _normals, unsigned int* _indices, unsigned int _indicesCount) 
		:vertices(_vertices), verticesCount(_verticesCount), normals(_normals), indices(_indices), indicesCount(_indicesCount)
	{
	}

	~Shape()
	{
		if (vertices) {
			delete [] vertices;
		}
		if (normals) {
			delete [] normals;
		}
		if (indices) {
			delete [] indices;
		}
	}
};

typedef boost::shared_ptr<Shape> ShapePtr;

class ShapeFactory
{
public:
	ShapeFactory(void);
	~ShapeFactory(void);

	ShapePtr createPlane(float halfExtent);
	ShapePtr createBox(float halfExtent);
	ShapePtr createScreenQuad(float zComponent);
};

