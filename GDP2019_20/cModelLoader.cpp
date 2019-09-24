#include "cModelLoader.hpp"

#include <fstream>
#include <iostream>

cModelLoader::cModelLoader()
{
}

cModelLoader::~cModelLoader()
{
}

bool cModelLoader::loadModel(std::string filename, cMesh* mesh)
{
	std::ifstream infile(filename);
	if (!infile)
		return false;		

	unsigned verts, faces;
	std::string t;
	while (infile >> t && t != "vertex");
	infile >> verts;
	
	while (infile >> t && t != "face");
	infile >> faces;

	while (infile >> t && t != "end_header");

	mesh->vecVertices.resize(verts);
	mesh->vecTriangles.resize(faces);

	for (unsigned i = 0; i < verts; ++i)
	{
		infile >> mesh->vecVertices[i].x >> mesh->vecVertices[i].y >> mesh->vecVertices[i].z
				>> mesh->vecVertices[i].nx >> mesh->vecVertices[i].ny >> mesh->vecVertices[i].nz;
	}

	unsigned v; // Ignore the first number
	for (unsigned i = 0; i < faces; ++i)
	{
		infile >> v >> mesh->vecTriangles[i].vert_index_1 >> mesh->vecTriangles[i].vert_index_2 >> mesh->vecTriangles[i].vert_index_3;
	}

	return true;
}
