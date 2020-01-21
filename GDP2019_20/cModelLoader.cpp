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

bool cModelLoader::loadModel(std::string filename, cMesh* mesh)
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

	mesh->vecVertices.resize(verts);
	mesh->vecTriangles.resize(faces);

	for (unsigned i = 0; i < verts; ++i)
	{
		infile >> mesh->vecVertices[i].x >> mesh->vecVertices[i].y >> mesh->vecVertices[i].z
			>> mesh->vecVertices[i].nx >> mesh->vecVertices[i].ny >> mesh->vecVertices[i].nz;
		if (UVs)
			infile >> mesh->vecVertices[i].u >> mesh->vecVertices[i].v;
	}

	unsigned v; // Ignore the first number
	for (unsigned i = 0; i < faces; ++i)
	{
		infile >> v >> mesh->vecTriangles[i].vert_index_1 >> mesh->vecTriangles[i].vert_index_2 >> mesh->vecTriangles[i].vert_index_3;
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
	if (errors != "")
	{
		delete pScene;
		return false;
	}

	if (pScene->HasMeshes())
	{
		//aiMesh* pMesh = pScene->mMeshes[0];
		/*mesh->vecTriangles.resize(pMesh->mNumFaces);
		for (size_t i = 0; i < pMesh->mNumFaces; ++i)
		{
			mesh->vecTriangles[i] = sPlyTriangle{
					pMesh->mFaces[i].mIndices[0],
					pMesh->mFaces[i].mIndices[1],
					pMesh->mFaces[i].mIndices[2]
				};
		}
		mesh->vecVertices.resize(pMesh->mNumVertices);
		for (size_t i = 0; i < pMesh->mNumVertices; ++i)
		{
			mesh->vecVertices[i] = sPlyVertex{
				pMesh->mVertices[i].x,
				pMesh->mVertices[i].y,
				pMesh->mVertices[i].z,
				pMesh->mNormals[i].x,
				pMesh->mNormals[i].y,
				pMesh->mNormals[i].z,
				pMesh->mTextureCoords[i]->x,
				pMesh->mTextureCoords[i]->y
			};
		}*/
		mesh->vecVertices.reserve(pScene->mMeshes[0]->mNumVertices);
		if (pScene->mMeshes[0]->HasTextureCoords(0))
		{
			for (size_t i = 0; i < pScene->mMeshes[0]->mNumVertices; ++i)
			{
				mesh->vecVertices.push_back(sPlyVertex{
					// Vert XYZ
					pScene->mMeshes[0]->mVertices[i].x,
					pScene->mMeshes[0]->mVertices[i].y,
					pScene->mMeshes[0]->mVertices[i].z,

					// Normal XYZ
					pScene->mMeshes[0]->mNormals[i].x,
					pScene->mMeshes[0]->mNormals[i].y,
					pScene->mMeshes[0]->mNormals[i].z,

					// Texture coords ST/UV
					pScene->mMeshes[0]->mTextureCoords[0][i].x,
					pScene->mMeshes[0]->mTextureCoords[0][i].y
					});
			}
		}
		else
		{
			for (size_t i = 0; i < pScene->mMeshes[0]->mNumVertices; ++i)
			{
				mesh->vecVertices.push_back(sPlyVertex{
					// Vert XYZ
					pScene->mMeshes[0]->mVertices[i].x,
					pScene->mMeshes[0]->mVertices[i].y,
					pScene->mMeshes[0]->mVertices[i].z,

					// Normal XYZ
					pScene->mMeshes[0]->mNormals[i].x,
					pScene->mMeshes[0]->mNormals[i].y,
					pScene->mMeshes[0]->mNormals[i].z,

					// Texture coords ST/UV
					0.0f,
					0.0f
					});
			}
		}
		

		mesh->vecTriangles.reserve(pScene->mMeshes[0]->mNumFaces);
		for (size_t i = 0; i < pScene->mMeshes[0]->mNumFaces; ++i)
		{
			mesh->vecTriangles.push_back(sPlyTriangle{
				pScene->mMeshes[0]->mFaces[i].mIndices[0],
				pScene->mMeshes[0]->mFaces[i].mIndices[1],
				pScene->mMeshes[0]->mFaces[i].mIndices[2]
				});
		}
	}
	else
	{
		errors = "No meshes";
	}

	//delete pScene;
	return true;
}
