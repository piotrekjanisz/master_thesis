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
	float* tangents;
	float* bitangents;
	float* texCoords;

	unsigned int indicesCount;
	unsigned int verticesCount;
	PrimitiveType primitiveType;

	Shape(float* _vertices, unsigned int _verticesCount, float* _normals, unsigned int* _indices, unsigned int _indicesCount, float* _texCoords = 0, PrimitiveType _primitiveType = TRIANGLES) 
		:vertices(_vertices), verticesCount(_verticesCount), normals(_normals), tangents(0), bitangents(0), indices(_indices), indicesCount(_indicesCount), texCoords(_texCoords), primitiveType(_primitiveType)
	{
	}

	Shape(float* _vertices, unsigned int _verticesCount, float* _normals, float* _tangents, float* _bitangents, unsigned int* _indices, unsigned int _indicesCount, float* _texCoords = 0, PrimitiveType _primitiveType = TRIANGLES)
		:vertices(_vertices), verticesCount(_verticesCount), normals(_normals), tangents(_tangents), bitangents(_bitangents), indices(_indices), indicesCount(_indicesCount), texCoords(_texCoords), primitiveType(_primitiveType)
	{
	}

	Shape(unsigned int _verticesCount, unsigned int _indicesCount)
		: verticesCount(_verticesCount), indicesCount(_indicesCount), vertices(0), normals(0), tangents(0), bitangents(0), texCoords(0), indices(0), primitiveType(TRIANGLES)
	{
		vertices = new float[verticesCount * 4];
		normals = new float[verticesCount * 3];
		indices = new unsigned int[indicesCount];
	}

	~Shape()
	{
		if (vertices) {
			delete [] vertices;
		}
		if (normals) {
			delete [] normals;
		}
		if (tangents) {
			delete [] tangents;
		}
		if (bitangents) {
			delete [] bitangents;
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

	ShapePtr createPlane(float halfExtent, float texCoordMult, int tiles);
	ShapePtr createPlane(float halfExtent, float texCoorMult);
	ShapePtr createBox(float halfExtent);
	ShapePtr createSkyBox(float halfExtent);
	ShapePtr createScreenQuad(float zComponent);
};

