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

	Shape(unsigned int _verticesCount, unsigned int _indicesCount)
		: verticesCount(_verticesCount), indicesCount(_indicesCount), vertices(0), normals(0), texCoords(0), indices(0), primitiveType(TRIANGLES)
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
		if (indices) {
			delete [] indices;
		}
		if (texCoords) {
			delete[] texCoords;
		}
	}
};

struct TriangleMesh
{
	float* vertices;
	float* normals;
	unsigned int* indices;
	
	int verticesCount;
	int trianglesCount;

	int maxVertices;
	int maxTriangles;

	TriangleMesh(int _maxVertices, int _maxTriangles)
		: maxVertices(_maxVertices), maxTriangles(_maxTriangles), verticesCount(0), trianglesCount(0), vertices(0), normals(0), indices(0)
	{
		vertices = new float[_maxVertices * 4];
		normals = new float[_maxVertices * 3];
		indices = new unsigned int[_maxTriangles * 3];
	}

	~TriangleMesh()
	{
		if (vertices)
			delete [] vertices;
		if (normals) 
			delete [] normals;
		if (indices) 
			delete [] indices;
	}
};

typedef boost::shared_ptr<Shape> ShapePtr;
typedef boost::shared_ptr<TriangleMesh> TriangleMeshPtr;

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

