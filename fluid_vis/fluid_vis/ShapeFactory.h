#pragma once

#include <boost/smart_ptr.hpp>

struct Shape
{
	enum PrimitiveType {
		TRIANGLES,
		QUADS
	};

	unsigned int* indices;
	float* vertices;
	float* normals;
	float* texCoords;
	unsigned int indicesCount;
	unsigned int verticesCount;
	PrimitiveType primitiveType;

	Shape(float* _vertices, unsigned int _verticesCount, float* _normals, unsigned int* _indices, unsigned int _indicesCount, float* _texCoords = 0, PrimitiveType _primitiveType = TRIANGLES) 
		:vertices(_vertices), verticesCount(_verticesCount), normals(_normals), indices(_indices), indicesCount(_indicesCount), texCoords(_texCoords), primitiveType(_primitiveType)
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
		if (texCoords) {
			delete[] texCoords;
		}
	}
};

typedef boost::shared_ptr<Shape> ShapePtr;

class ShapeFactory
{
public:
	ShapeFactory(void);
	~ShapeFactory(void);

	ShapePtr createPlane(float halfExtent, float texCoorMult);
	ShapePtr createBox(float halfExtent);
	ShapePtr createSkyBox(float halfExtent);
	ShapePtr createScreenQuad(float zComponent);
};

