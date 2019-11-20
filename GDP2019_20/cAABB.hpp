#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include <vector>
#include <map>
#include "cMesh.hpp"			// for the triangle information
#include <iostream>

void CalcAABBsForMesh(cMesh* mesh, float AABBsize, unsigned xcount, unsigned ycount, unsigned zcount);

class cAABB
{
public:
	static std::map<unsigned long long, cAABB*> g_mapAABBs_World;

	cAABB() {};
	glm::vec3 min;
	glm::vec3 max;

	cMesh* pTheMesh;
	std::vector<sPlyTriangle*> vecTriangles;

	static unsigned long long get_ID_of_AABB_I_Might_Be_In(glm::vec3 testLocation)
	{
		if (testLocation.x < 0.0f)
			testLocation.x -= 100.0f;
		if (testLocation.y < 0.0f)
			testLocation.y -= 100.0f;
		if (testLocation.z < 0.0f)
			testLocation.z -= 100.0f;
		return cAABB::calcID(testLocation);
	}

	static unsigned long long calcID(glm::vec3 testLocation);

	// This returns the ID the specific AABB
	unsigned long long getID()
	{
		return cAABB::calcID(this->min);
	}

	bool isVertInside(glm::vec3 VertexXYZ);
};
