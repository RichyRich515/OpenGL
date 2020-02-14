#include "cModelLoader.hpp"

#include <fstream>
#include <iostream>

#include <assimp/config.h>

#include <assimp/Importer.hpp>		// To load from a file
#include <assimp/scene.h>			// To handle the scene, mesh, etc. object
#include <assimp/postprocess.h>		// For generating normals, etc.


cModelLoader::cModelLoader()
{
}

cModelLoader::~cModelLoader()
{
}

bool cModelLoader::loadModel_ply(std::string filename, cMesh* mesh)
{
	std::ifstream infile(filename);
	if (!infile)
		return false;

	unsigned verts, faces;
	std::string t;
	while (infile >> t && t != "vertex");
	infile >> verts;

	bool UVs = false;
	while (infile >> t && t != "face")
	{
		if (t == "s" || t == "t" || t == "u" || t == "v" || t == "texture_u" || t == "texture_v")
			UVs = true;
	}
	infile >> faces;

	while (infile >> t && t != "end_header");

	mesh->vertices = new cVertex[verts];
	mesh->triangles = new cIndexedTriangle[faces];

	for (unsigned i = 0; i < verts; ++i)
	{
		infile >> mesh->vertices[i].x >> mesh->vertices[i].y >> mesh->vertices[i].z
			>> mesh->vertices[i].nx >> mesh->vertices[i].ny >> mesh->vertices[i].nz;
		if (UVs)
			infile >> mesh->vertices[i].u0 >> mesh->vertices[i].v0;
	}

	unsigned v; // Ignore the first number
	for (unsigned i = 0; i < faces; ++i)
	{
		infile >> v >> mesh->triangles[i].vert_index_1 >> mesh->triangles[i].vert_index_2 >> mesh->triangles[i].vert_index_3;
	}

	return true;
}

bool cModelLoader::loadModel_assimp(std::string filename, cMesh* mesh, std::string errors)
{
	const struct aiScene* pScene = NULL;

	Assimp::Importer mImporter;

	unsigned int Flags = aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace;

	pScene = mImporter.ReadFile(filename.c_str(), Flags);

	errors.clear();
	errors.append(mImporter.GetErrorString());
	if (errors.length() > 0)
	{
		delete pScene;
		return false;
	}

	if (pScene->HasMeshes())
	{
		mesh->numberOfVertices = pScene->mMeshes[0]->mNumVertices;
		mesh->vertices = new cVertex[mesh->numberOfVertices];
		bool has_normals = pScene->mMeshes[0]->HasNormals();
		bool has_UV0 = pScene->mMeshes[0]->HasTextureCoords(0);

		for (size_t i = 0; i < mesh->numberOfVertices; ++i)
		{
			// Vert XYZ
			mesh->vertices[i].x = pScene->mMeshes[0]->mVertices[i].x;
			mesh->vertices[i].y = pScene->mMeshes[0]->mVertices[i].y;
			mesh->vertices[i].z = pScene->mMeshes[0]->mVertices[i].z;

			if (has_normals)
			{
				mesh->vertices[i].nx = pScene->mMeshes[0]->mNormals[i].x;
				mesh->vertices[i].ny = pScene->mMeshes[0]->mNormals[i].y;
				mesh->vertices[i].nz = pScene->mMeshes[0]->mNormals[i].z;
			}

			if (has_UV0)
			{
				// Texture coords ST/UV
				mesh->vertices[i].u0 = pScene->mMeshes[0]->mTextureCoords[0][i].x;
				mesh->vertices[i].v0 = pScene->mMeshes[0]->mTextureCoords[0][i].y;
			}
		}

		mesh->numberOfTriangles = pScene->mMeshes[0]->mNumFaces;
		mesh->triangles = new cIndexedTriangle[mesh->numberOfTriangles];
		for (size_t i = 0; i < mesh->numberOfTriangles; ++i)
		{
			mesh->triangles[i].vert_index_1 = pScene->mMeshes[0]->mFaces[i].mIndices[0];
			mesh->triangles[i].vert_index_2 = pScene->mMeshes[0]->mFaces[i].mIndices[1];
			mesh->triangles[i].vert_index_3 = pScene->mMeshes[0]->mFaces[i].mIndices[2];
		}
	}
	else
	{
		errors = "No meshes";
	}

	return true;
}
