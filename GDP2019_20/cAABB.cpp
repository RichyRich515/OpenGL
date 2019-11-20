#include "cAABB.hpp"

#include <iostream>

std::map<unsigned long long, cAABB*> cAABB::g_mapAABBs_World;

void CalcAABBsForMesh(cMesh* mesh, float AABBsize, unsigned xcount, unsigned ycount, unsigned zcount)
{
	float xx = xcount / 2;
	float yy = ycount / 2;
	float zz = zcount / 2;

	for (float x = -xx; x < xx; ++x)
	{
		for (float y = -yy; y < yy; ++y)
		{
			for (float z = -zz; z < zz; ++z)
			{
				cAABB* pAABB = new cAABB();
				pAABB->min = glm::vec3(x * AABBsize, y * AABBsize, z * AABBsize);
				pAABB->max = glm::vec3(pAABB->min.x + AABBsize, pAABB->min.y + AABBsize, pAABB->min.z + AABBsize);
				unsigned long long id = pAABB->getID();
				// Save the mesh to look up triangles. 
				// NOTE: you likely don't want to do this.
				// You likely want to make a new structure just for this
				// (the PlyTriangles store and indes to the vertices, which is not needed)

				// Store a pointer to the mesh
				pAABB->pTheMesh = mesh;

				cAABB::g_mapAABBs_World[id] = pAABB;
			}
		}
	}

	// Check which AABB this triangle is in

	for (std::vector<sPlyTriangle>::iterator itTri = mesh->vecTriangles.begin();
		itTri != mesh->vecTriangles.end(); 
		++itTri)
	{
		// TODO: If triangle is too big slice the triangle and try again
		glm::vec3 v1;
		v1.x = mesh->vecVertices[itTri->vert_index_1].x;
		v1.y = mesh->vecVertices[itTri->vert_index_1].y;
		v1.z = mesh->vecVertices[itTri->vert_index_1].z;
		glm::vec3 v2;
		v2.x = mesh->vecVertices[itTri->vert_index_2].x;
		v2.y = mesh->vecVertices[itTri->vert_index_2].y;
		v2.z = mesh->vecVertices[itTri->vert_index_2].z;
		glm::vec3 v3;
		v3.x = mesh->vecVertices[itTri->vert_index_3].x;
		v3.y = mesh->vecVertices[itTri->vert_index_3].y;
		v3.z = mesh->vecVertices[itTri->vert_index_3].z;

		// Get the ID of the AABB that this vertex is inside of 
		unsigned long long ID_AABB_V1 = cAABB::get_ID_of_AABB_I_Might_Be_In(v1);
		unsigned long long ID_AABB_V2 = cAABB::get_ID_of_AABB_I_Might_Be_In(v2);
		unsigned long long ID_AABB_V3 = cAABB::get_ID_of_AABB_I_Might_Be_In(v3);

		// Add this triangle to that box
		// sPlyTriangle& refTriangle = *itTri;
		// so &refTriangle returns the pointer to the triangle in the vector

		auto itAABB = cAABB::g_mapAABBs_World.find(ID_AABB_V1);
		if (itAABB != cAABB::g_mapAABBs_World.end())
		{
			cAABB::g_mapAABBs_World[ID_AABB_V1]->vecTriangles.push_back(&(*itTri));
		}
		itAABB = cAABB::g_mapAABBs_World.find(ID_AABB_V2);
		if (itAABB != cAABB::g_mapAABBs_World.end())
		{
			cAABB::g_mapAABBs_World[ID_AABB_V2]->vecTriangles.push_back(&(*itTri));
		}
		itAABB = cAABB::g_mapAABBs_World.find(ID_AABB_V3);
		if (itAABB != cAABB::g_mapAABBs_World.end())
		{
			cAABB::g_mapAABBs_World[ID_AABB_V3]->vecTriangles.push_back(&(*itTri));
		}
	}
}

unsigned long long cAABB::calcID(glm::vec3 testLocation)
{
	unsigned long long ulX = (((unsigned long long) fabs(testLocation.x)) / 100);// + 100;
	unsigned long long ulY = (((unsigned long long) fabs(testLocation.y)) / 100);// + 100;
	unsigned long long ulZ = (((unsigned long long) fabs(testLocation.z)) / 100);// + 100;

	// Are these negative
	if (testLocation.x < 0.0f)
	{
		ulX += 100000;
	}
	if (testLocation.y < 0.0f)
	{
		ulY += 100000;
	}
	if (testLocation.z < 0.0f)
	{
		ulZ += 100000;
	}

	return ulX * 1'000'000'000'000 + ulY * 1'000'000 + ulZ;
} 

bool cAABB::isVertInside(glm::vec3 VertexXYZ)
{
	if (VertexXYZ.x < min.x) return false;
	if (VertexXYZ.x > max.x) return false;
	if (VertexXYZ.y < min.y) return false;
	if (VertexXYZ.y > max.y) return false;
	if (VertexXYZ.z < min.z) return false;
	if (VertexXYZ.z > max.z) return false;

	// It's inside!
	return true;
}