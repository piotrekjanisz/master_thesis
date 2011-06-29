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

	enum {
		EDGE_BOTTOM_BACK = 1,
		EDGE_BOTTOM_RIGHT = 2,
		EDGE_BOTTOM_FRONT = 4,
		EDGE_BOTTOM_LEFT = 8,
		EDGE_TOP_BACK = 16,
		EDGE_TOP_RIGHT = 32,
		EDGE_TOP_FRONT = 64,
		EDGE_TOP_LEFT = 128
	};

	void polygonize(CornerCacheEntry** corners, double isoTreshold, float* vertices, int vertexComponents, unsigned int* indices, unsigned int indicesStart, int& ntriag, int& nvert, int& cubesToDo);
	int cubesToDo(CornerCacheEntry** corners, double isoTreshold);

	Polygonizer(void);
	~Polygonizer(void);
};

