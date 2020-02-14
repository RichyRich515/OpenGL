#include "cVAOManager.hpp"

#include "GLCommon.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <sstream>

cVAOManager* cVAOManager::currentVAO;

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = nullptr;
	this->pIndices = nullptr;

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	glm::vec3 maxValues;
	glm::vec3 minValues;

	//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;

	return;
}


cVAOManager::~cVAOManager()
{
	for (auto v : this->m_map_ModelName_to_VAOID)
	{
		delete v.second.pIndices;
	}
}

bool cVAOManager::LoadModelIntoVAO(std::string name, cMesh* mesh, unsigned int shaderProgramID)
{
	sModelDrawInfo drawInfo;
	drawInfo.meshName = name;
	drawInfo.numberOfVertices = mesh->numberOfVertices;
	drawInfo.pVertices = mesh->vertices;
	drawInfo.numberOfTriangles = mesh->numberOfTriangles;
	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;
	drawInfo.pIndices = new unsigned[drawInfo.numberOfIndices];
	for (unsigned i = 0, j = 0; i != drawInfo.numberOfTriangles; ++i, j += 3)
	{
		drawInfo.pIndices[j + 0] = mesh->triangles[i].vert_index_1;
		drawInfo.pIndices[j + 1] = mesh->triangles[i].vert_index_2;
		drawInfo.pIndices[j + 2] = mesh->triangles[i].vert_index_3;
	}


	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	glGenVertexArrays(1, &(drawInfo.VAO_ID));
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 

	glGenBuffers(1, &(drawInfo.VertexBufferID));

	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cVertex) * drawInfo.numberOfVertices, (GLvoid*)drawInfo.pVertices, GL_DYNAMIC_DRAW);

	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers(1, &(drawInfo.IndexBufferID));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * drawInfo.numberOfIndices, (GLvoid*)drawInfo.pIndices, GL_STATIC_DRAW);

	// Set the vertex attributes.
	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPosition");
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vColour");
	GLint vnorm_location = glGetAttribLocation(shaderProgramID, "vNormal");
	GLint vUV_location = glGetAttribLocation(shaderProgramID, "vUVx2");

	GLint vTangent_location = glGetAttribLocation(shaderProgramID, "vTangent");
	GLint vBiNormal_location = glGetAttribLocation(shaderProgramID, "vBiNormal");
	GLint vBoneID_location = glGetAttribLocation(shaderProgramID, "vBoneID");
	GLint vBoneWeight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, x)));

	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, r)));

	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 4, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, nx)));

	glEnableVertexAttribArray(vUV_location);
	glVertexAttribPointer(vUV_location, 4, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, u0)));

	glEnableVertexAttribArray(vTangent_location);
	glVertexAttribPointer(vTangent_location, 4, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, tx)));

	glEnableVertexAttribArray(vBiNormal_location);
	glVertexAttribPointer(vBiNormal_location, 4, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, bx)));

	glEnableVertexAttribArray(vBoneID_location);
	glVertexAttribPointer(vBoneID_location, NUMBER_OF_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, boneID[0])));

	glEnableVertexAttribArray(vBoneWeight_location);
	glVertexAttribPointer(vBoneWeight_location, NUMBER_OF_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(cVertex), (void*)(offsetof(cVertex, boneWeights[0])));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vnorm_location);
	glDisableVertexAttribArray(vUV_location);

	glDisableVertexAttribArray(vTangent_location);
	glDisableVertexAttribArray(vBiNormal_location);
	glDisableVertexAttribArray(vBoneID_location);
	glDisableVertexAttribArray(vBoneWeight_location);

	this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;

	return true;
}

bool cVAOManager::FindDrawInfoByModelName(std::string filename, sModelDrawInfo& drawInfo)
{
	auto itDrawInfo = this->m_map_ModelName_to_VAOID.find(filename);
	if (itDrawInfo != this->m_map_ModelName_to_VAOID.end())
	{
		drawInfo = itDrawInfo->second;
		return true;
	}
	return false;

}