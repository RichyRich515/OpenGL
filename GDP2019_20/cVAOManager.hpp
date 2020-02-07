// cVAOManager.hpp
// VAO manager class
// Richard Woods
// 2019-09-10

#pragma once

#include <string>
#include <map>

#include "cMesh.hpp" // model from file

struct sVertex
{
	float x, y, z, w; // w coordinate	
	float r, g, b, a; // a = alpha (transparency)
	float nx, ny, nz, nw;
	float u0, v0, u1, v1;
};

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned* pIndices;
};


class cVAOManager
{
public:

	// Takes cMesg object and loads into GPU (as a VAO)
	bool LoadModelIntoVAO(std::string name, cMesh* mesh, unsigned int shaderProgramID);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename, sModelDrawInfo& drawInfo);

	std::string getLastError(bool bAndClear = true);


	static void setCurrentVAOManager(cVAOManager* v) { currentVAO = v; }
	static cVAOManager* getCurrentVAOManager() { return currentVAO; }

private:
	static cVAOManager* currentVAO;

	std::map<std::string, sModelDrawInfo> m_map_ModelName_to_VAOID;
};