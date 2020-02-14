#include "cMesh.hpp"

cVertex::cVertex() :
	x(0.0f), y(0.0f), z(0.0f), w(1.0f),
	r(0.0f), g(0.0f), b(0.0f), a(1.0f),		// Note alpha is 1.0
	nx(0.0f), ny(0.0f), nz(0.0f), nw(1.0f),
	u0(0.0f), v0(0.0f), u1(0.0f), v1(0.0f),
	tx(0.0f), ty(0.0f), tz(0.0f), tw(1.0f),
	bx(0.0f), by(0.0f), bz(0.0f), bw(1.0f)
{
	memset(this->boneID, 0, sizeof(unsigned int) * NUMBER_OF_BONES_PER_VERTEX);
	memset(this->boneWeights, 0, sizeof(float) * NUMBER_OF_BONES_PER_VERTEX);
}

cIndexedTriangle::cIndexedTriangle() :
	vert_index_1(0),
	vert_index_2(0),
	vert_index_3(0)
{
}

cMesh::cMesh() :
	name(""),
	numberOfVertices(0),
	vertices(nullptr),
	numberOfTriangles(0),
	triangles(nullptr)
{
}

cMesh::~cMesh()
{
	if (vertices)
		delete[] vertices;
	if (triangles)
		delete[] triangles;
}
