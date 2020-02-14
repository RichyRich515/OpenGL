// cMesh.hpp
// Basic mesh class
// Richard Woods
// 2019-09-09

#pragma once

#include <string>

static const unsigned int NUMBER_OF_BONES_PER_VERTEX = 4;

class cVertex
{
public:
	cVertex();
	~cVertex() {}
	float x, y, z, w;
	float r, g, b, a;
	float nx, ny, nz, nw;
	float u0, v0, u1, v1;
	float tx, ty, tz, tw;
	float bx, by, bz, bw;

	float boneID[NUMBER_OF_BONES_PER_VERTEX];
	float boneWeights[NUMBER_OF_BONES_PER_VERTEX];
};

class cIndexedTriangle
{
public:
	cIndexedTriangle();
	~cIndexedTriangle() {}

	unsigned vert_index_1;
	unsigned vert_index_2;
	unsigned vert_index_3;
};

class cMesh
{
public:
	cMesh();
	~cMesh();

	std::string name;

	unsigned numberOfVertices;
	cVertex* vertices;

	unsigned numberOfTriangles;
	cIndexedTriangle* triangles;
};