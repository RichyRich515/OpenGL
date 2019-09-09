// cMesh.hpp
// Basic mesh class
// Richard Woods
// 2019-09-09

#pragma once

#include <vector>

// Matches ply file format
struct sPlyVertex
{
	float x, y, z;
};

struct sPlyTriangle
{
	unsigned vert_index_1;
	unsigned vert_index_2;
	unsigned vert_index_3;
};

class cMesh
{
public:
	cMesh() {}
	~cMesh() {}
	
	std::vector<sPlyVertex> vecVertices;
	std::vector<sPlyTriangle> vecTriangles;
};