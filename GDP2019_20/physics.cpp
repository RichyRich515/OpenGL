// Physics.cpp
// Physics stuff
// Richard Woods
// 2019-09-19

#include "Physics.hpp"

#include <vector>
#include "cGameObject.hpp"
#include <glm/glm.hpp>

#include <iostream>

// closest point in 3D space.
glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}

struct Sphere
{
	glm::vec3 c;
	float r;
};


int TestSphereTriangle(Sphere s, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3& p)
{
	// Find point P on triangle ABC closest to sphere center
	p = ClosestPtPointTriangle(s.c, a, b, c);

	// Sphere and triangle intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	glm::vec3 v = p - s.c;
	return glm::dot(v, v) <= s.r * s.r;
}


void physicsUpdate(std::vector<cGameObject*>& vecGameObjects, float dt)
{
	for (auto itr = vecGameObjects.begin(); itr != vecGameObjects.end(); ++itr)
	{
		if ((*itr)->inverseMass == 0.0f)
			continue;

		cGameObject* go = *itr;

		go->velocity.x += go->acceleration.x * dt;
		go->velocity.y += go->acceleration.y * dt;
		go->velocity.z += go->acceleration.z * dt;

		go->position.x += go->velocity.x * dt;
		go->position.y += go->velocity.y * dt;
		go->position.z += go->velocity.z * dt;

		// Test to see if hit ground plane

		//if ((go->position.y - 1.0f) <= 0.0f)
		//	go->velocity.y = fabs(go->velocity.y) * 0.95; // Lose some velocity


		glm::vec3 closetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		float closestDistanceSoFar = FLT_MAX;

		// Test for all other Tris
		for (auto itr2 = vecGameObjects.begin(); itr2 != vecGameObjects.end(); ++itr2)
		{
			if (itr2 == itr) // Dont check self
				continue;

			cGameObject* go2 = *itr2;

			cMesh* mesh = go2->mesh;
			if (!mesh)
				continue;


			for (unsigned i = 0; i < mesh->vecVertices.size(); ++i)
			{

				glm::vec3 p1 = glm::vec3(
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_1].x,
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_1].y,
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_1].z);

				glm::vec3 p2 = glm::vec3(
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_2].x,
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_2].y,
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_2].z);

				glm::vec3 p3 = glm::vec3(
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_3].x,
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_3].y,
					mesh->vecVertices[mesh->vecTriangles[i].vert_index_3].z);

				glm::vec3 curClosetPoint = ClosestPtPointTriangle(go->position, p1, p2, p3);


				// Is this the closest so far?
				float distanceNow = glm::distance(curClosetPoint, go->position);

				// is this closer than the closest distance
				if (distanceNow <= closestDistanceSoFar)
				{
					closestDistanceSoFar = distanceNow;
					closetPoint = curClosetPoint;
				}
			}
		}

		//std::cout << closestDistanceSoFar << std::endl;
		// Bounce based on closest point if collision (need to do normals and cross product stuff here probably)
		if ((closestDistanceSoFar - 0.7f) < 0.01 )
		{
			go->velocity.y = abs(go->velocity.y);
		}

	}
}