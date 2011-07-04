#pragma once

#include "Slice.h"

/*
 * vertices numbering
 *     4-----5
 *    /|    /| 
 *   / |   / |         ^y  
 *  7-----6  |         |
 *  |  |  |  |         |
 *  |  |  |  |         |         x
 *  |  0--|--1         |--------->
 *  | /   | /         /
 *  |/    |/       z /
 *  3-----2         v
 *
 */

/*
 * edge numbering
 *        4
 *     *-----*
 *   7/|   5/| 
 *   / |6  / |         ^y  
 *  *-----*  |         |
 *  | 8|  | 9|         |
 *  |  | 0|  |         |         x
 *11|  *--|--*         |--------->
 *  | / 10| /         /
 *  |/3   |/ 1     z /
 *  *-----*         v
 *     2
 */

class Polygonizer
{
private:
	static int EDGE_TABLE[256];
	static int TODO_TABLE[256];
	static int TRI_TABLE[256][16];

	void vertexInterpolate(double isoTreshold, float* p1, float* p2, double isoVal1, double isoVal2, float* result);

	void addVertex(float* p1, float* p2, double isoVal1, double isoVal2);

    double _isoTreshold;
    int _vertexComponents;
    float* _vertices;
    float* _currentVertex;
    unsigned int _currentIndex;
    unsigned int* _indices;
    int _ntriag;
    int _nvert;

public:

	enum 
	{
		CUBE_BELOW = 1,
		CUBE_ABOVE = 2,
		CUBE_LEFT  = 4,
		CUBE_RIGHT = 8,
		CUBE_BACK  = 16,
		CUBE_FRONT = 32
	};

	Polygonizer(void);
    Polygonizer(double isoTreshold, float* vertices, int vertexComponents, unsigned int* indices)
        : _isoTreshold(isoTreshold), _vertices(vertices), _vertexComponents(vertexComponents), _indices(indices), _currentIndex(0), _currentVertex(vertices), _ntriag(0), _nvert(0)
    {
    }
	
	~Polygonizer(void);

	int cubesToDo(CornerCacheEntry** corners);
    void polygonize(CornerCacheEntry** corners, int& cubesToDo);

    int getTriangleNumber() { return _ntriag; }
    int getVerticesNumber() { return _currentIndex; }
};

