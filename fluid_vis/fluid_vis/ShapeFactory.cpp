#include "ShapeFactory.h"

#include <boost/make_shared.hpp>
#include <GL/glus.h>


ShapeFactory::ShapeFactory(void)
{
}


ShapeFactory::~ShapeFactory(void)
{
}

ShapePtr ShapeFactory::createPlane(float halfExtent, float texCoorMult)
{
	const unsigned int VERTICES_COUNT = 5;
	const unsigned int INDICES_COUNT = 4 * 3;
	const unsigned int TEX_COORDS_COUNT = 5;
	float* vertices = new float[VERTICES_COUNT * 4];
	float* normals = new float[VERTICES_COUNT * 3];
	float* tex_coords = new float[TEX_COORDS_COUNT * 2];
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

	i = 0;
	tex_coords[i++] = 0.0f;               tex_coords[i++] = 0.0f;
	tex_coords[i++] = texCoorMult;        tex_coords[i++] = 0.0f;
	tex_coords[i++] = 0.0f;               tex_coords[i++] = texCoorMult;
	tex_coords[i++] = texCoorMult;        tex_coords[i++] = texCoorMult;
	tex_coords[i++] = 0.5f * texCoorMult; tex_coords[i++] = 0.5f * texCoorMult;

	return boost::make_shared<Shape>(vertices, VERTICES_COUNT, normals, indices, INDICES_COUNT, tex_coords);
}

ShapePtr ShapeFactory::createBox(float halfExtent)
{
	GLUSshape box;
	glusCreateCubef(&box, halfExtent);
	return boost::make_shared<Shape>(box.vertices, box.numberVertices, box.normals, box.indices, box.numberIndices, box.texCoords);
}

ShapePtr ShapeFactory::createScreenQuad(float zComponent)
{
	int vertices_data_size = 4 * 4;
	float* vertices = new float[vertices_data_size];
	
	/*
	 *	3---2
	 *  |   |		   ^y	  
	 *  |   |          | 
	 *  |   |          |     x
	 *  0---1          ------>
	 *                
	 */ 

	int i = 0;
	vertices[i++] = -1.0f; vertices[i++] = -1.0f; vertices[i++] = zComponent; vertices[i++] = 1.0f; // 0
	vertices[i++] =  1.0f; vertices[i++] = -1.0f; vertices[i++] = zComponent; vertices[i++] = 1.0f; // 1
	vertices[i++] =  1.0f; vertices[i++] =  1.0f; vertices[i++] = zComponent; vertices[i++] = 1.0f; // 2
	vertices[i++] = -1.0f; vertices[i++] =  1.0f; vertices[i++] = zComponent; vertices[i++] = 1.0f; // 3
	
	return boost::make_shared<Shape>(vertices, 4, (float*)NULL, (unsigned int*)NULL, (unsigned int)0);
}

void createSkyBox2(float halfExtent)
{
	float vertices[] = {
		// negative z
		-halfExtent,  halfExtent, -halfExtent, 1.0, 
		-halfExtent, -halfExtent, -halfExtent, 1.0, 
		 halfExtent, -halfExtent, -halfExtent, 1.0, 
		 halfExtent,  halfExtent, -halfExtent, 1.0, 
		// posivite z
		-halfExtent,  halfExtent,  halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent,  halfExtent, 1.0,
		// positive x
		 halfExtent,  halfExtent, -halfExtent, 1.0,
		 halfExtent, -halfExtent, -halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		// negative x
		-halfExtent,  halfExtent, -halfExtent, 1.0,
		-halfExtent,  halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent, -halfExtent, 1.0,
		// positive y
		-halfExtent,  halfExtent,  halfExtent, 1.0,
		-halfExtent,  halfExtent, -halfExtent, 1.0,
		 halfExtent,  halfExtent, -halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		// negative y
		-halfExtent, -halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent, -halfExtent, 1.0,
		-halfExtent, -halfExtent, -halfExtent, 1.0,
	};

		unsigned int indices[] = {
		 0,  1,  2,  3,      // negative z
		 4,  5,  6,  7,      // positive z
		 8,  9, 10, 11,      // positive x
		12, 13, 14, 15,      // negative x
		16, 17, 18, 19,      // positive y
		20, 21, 22, 23,      // negative y
	};


		/*
		
	float vertices[] = {
		// negative z
		-halfExtent,  halfExtent, -halfExtent, 1.0, 
		-halfExtent, -halfExtent, -halfExtent, 1.0, 
		 halfExtent, -halfExtent, -halfExtent, 1.0, 
		 halfExtent,  halfExtent, -halfExtent, 1.0, 
		// posivite z
		-halfExtent,  halfExtent,  halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent,  halfExtent, 1.0,
		// positive x
		 halfExtent,  halfExtent, -halfExtent, 1.0,
		 halfExtent, -halfExtent, -halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		// negative x
		-halfExtent,  halfExtent, -halfExtent, 1.0,
		-halfExtent,  halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent, -halfExtent, 1.0,
		// positive y
		-halfExtent,  halfExtent,  halfExtent, 1.0,
		-halfExtent,  halfExtent, -halfExtent, 1.0,
		 halfExtent,  halfExtent, -halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		// negative y
		-halfExtent, -halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent, -halfExtent, 1.0,
		-halfExtent, -halfExtent, -halfExtent, 1.0,
	};

	unsigned int indices[] = {
		 0,  1,  2,      // negative z
		 2,  3,  0,
		 4,  5,  6,      // positive z
		 6,  7,  4,
		 8,  9, 10,      // positive x
		10, 11,  8,
		12, 13, 14,      // negative x
		14, 15, 12,
		16, 17, 18,      // positive y
		18, 19, 16,
		20, 21, 22,      // negative y
		22, 23, 20
	};

	const unsigned int VERTICES_COUNT = 24;
	const unsigned int INDICES_COUNT = 36;

	float* retValVertices = new float[VERTICES_COUNT * 4];
	memcpy(retValVertices, vertices, VERTICES_COUNT * 4 * sizeof(float));

	unsigned int* retValIndices = new unsigned int[INDICES_COUNT];
	memcpy(retValIndices, indices, INDICES_COUNT * sizeof(unsigned int));

	return boost::make_shared<Shape>(retValVertices, VERTICES_COUNT, (float*) 0, retValIndices, (unsigned int)INDICES_COUNT, (float*) 0, Shape::TRIANGLES);
		*/
}

ShapePtr ShapeFactory::createSkyBox(float halfExtent)
{
	/*  walls are facing inside cube, winding is counter clockwise
	 *     0-----3
	 *    /|    /| 
	 *   / |   / |         ^y  
	 *  4-----5  |         |
	 *  |  |  |  |         |
	 *  |  |  |  |         |         x
	 *  |  1--|--2         |--------->
	 *  | /   | /         /
	 *  |/    |/       z /
	 *  7-----6         v
	 *
	 */

	float vertices[] = {
		-halfExtent,  halfExtent, -halfExtent, 1.0,
		-halfExtent, -halfExtent, -halfExtent, 1.0,
		 halfExtent, -halfExtent, -halfExtent, 1.0,
		 halfExtent,  halfExtent, -halfExtent, 1.0,

		-halfExtent,  halfExtent,  halfExtent, 1.0,
		 halfExtent,  halfExtent,  halfExtent, 1.0,
		 halfExtent, -halfExtent,  halfExtent, 1.0,
		-halfExtent, -halfExtent,  halfExtent, 1.0,
	};

	unsigned int indices[] = {
		0, 1, 2,    // negative z
		2, 3, 0,
		4, 5, 6,    // positive z
		6, 7, 4,
		4, 7, 1,    // negative x
		1, 0, 4,
		5, 3, 2,    // positive x
		2, 6, 5, 
		1, 7, 6,    // negative y 
		6, 2, 1,
		0, 3, 5,    // positive y
		5, 4, 0
	};

	const unsigned int VERTICES_COUNT = 8;
	const unsigned int INDICES_COUNT = 36;

	float* retValVertices = new float[VERTICES_COUNT * 4];
	memcpy(retValVertices, vertices, VERTICES_COUNT * 4 * sizeof(float));

	unsigned int* retValIndices = new unsigned int[INDICES_COUNT];
	memcpy(retValIndices, indices, INDICES_COUNT * sizeof(unsigned int));

	return boost::make_shared<Shape>(retValVertices, VERTICES_COUNT, (float*) 0, retValIndices, (unsigned int)INDICES_COUNT, (float*) 0, Shape::TRIANGLES);
}
