#include "cVAOManager.hpp"

#include "GLCommon.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <sstream>

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
	this->pVertices = NULL;
	this->pIndices = NULL;

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	glm::vec3 maxValues;
	glm::vec3 minValues;

//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;

	return;
}


bool cVAOManager::LoadModelIntoVAO(std::string name, cMesh* mesh, unsigned int shaderProgramID)
{
	sModelDrawInfo drawInfo;
	drawInfo.meshName = name;
	drawInfo.numberOfVertices = mesh->vecVertices.size();
	drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
	for (unsigned i = 0; i != drawInfo.numberOfVertices; ++i)
		drawInfo.pVertices[i] = sVertex{
			mesh->vecVertices[i].x, mesh->vecVertices[i].y, mesh->vecVertices[i].z, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			mesh->vecVertices[i].nx, mesh->vecVertices[i].ny, mesh->vecVertices[i].nz, 1.0f,
			mesh->vecVertices[i].u, mesh->vecVertices[i].v, 1.0f, 1.0f };

	drawInfo.numberOfTriangles = mesh->vecTriangles.size();
	drawInfo.numberOfIndices = mesh->vecTriangles.size() * 3;
	drawInfo.pIndices = new unsigned[drawInfo.numberOfIndices];
	for (unsigned i = 0, j = 0; i != mesh->vecTriangles.size(); ++i, j += 3)
	{
		drawInfo.pIndices[j + 0] = mesh->vecTriangles[i].vert_index_1;
		drawInfo.pIndices[j + 1] = mesh->vecTriangles[i].vert_index_2;
		drawInfo.pIndices[j + 2] = mesh->vecTriangles[i].vert_index_3;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(sVertex) * drawInfo.numberOfVertices, (GLvoid*) drawInfo.pVertices, GL_STATIC_DRAW);

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

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 4,	GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(offsetof(sVertex, x)));

	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(offsetof(sVertex, r)));

	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(offsetof(sVertex, nx)));

	glEnableVertexAttribArray(vUV_location);
	glVertexAttribPointer(vUV_location, 4, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(offsetof(sVertex, u0)));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);

	this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;

	return true;
}

// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}

