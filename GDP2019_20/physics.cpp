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

// Given point p, return the point on or in AABB that is closest to p
glm::vec3 ClosestPtPointAABB(glm::vec3 p, glm::vec3 min, glm::vec3 max)
{
	glm::vec3 q;
	// For each coordinate axis, if the point coordinate value is
	// outside box, clamp it to the box, else keep it as is
	for (int i = 0; i < 3; i++) 
	{
		float v = p[i];
		if (v < min[i]) 
			v = min[i];
		if (v > max[i]) 
			v = max[i];
		q[i] = v;
	}
	return q;
}


// Calculates the face normal of a tri
// From https://stackoverflow.com/questions/13689632/converting-vertex-normals-to-face-normals
glm::vec3 CalcNormalOfFace(glm::vec3 const& v1, glm::vec3 const& v2, glm::vec3 const& v3, glm::vec3 const& n1, glm::vec3 const& n2, glm::vec3 const& n3)
{
	glm::vec3 p0 = v2 - v1;
	glm::vec3 p1 = v3 - v1;
	glm::vec3 faceNormal = glm::normalize(glm::cross(p0, p1));

	glm::vec3 vertexNormal = (n1 + n2 + n3) / 3.0f; // or you can average 3 normals.
	float d = glm::dot(faceNormal, vertexNormal);

	return (d < 0.0f) ? -faceNormal : faceNormal;
}


void physicsUpdate(std::vector<cGameObject*>& vecGameObjects, glm::vec3 gravity, float dt, cDebugRenderer* debugRenderer, bool debug_mode)
{
	if (dt > MAX_PHYSICS_DELTA_TIME)
		dt = MAX_PHYSICS_DELTA_TIME;

	for (auto itr = vecGameObjects.begin(); itr != vecGameObjects.end(); ++itr)
	{
		if ((*itr)->inverseMass == 0.0f)
			continue;

		cGameObject* go = *itr;

		go->velocity.x += (go->acceleration.x + gravity.x) * dt ;
		go->velocity.y += (go->acceleration.y + gravity.y) * dt;
		go->velocity.z += (go->acceleration.z + gravity.z) * dt;
		

		go->position.x += go->velocity.x * dt;
		go->position.y += go->velocity.y * dt;
		go->position.z += go->velocity.z * dt;



		// Test for all other Tris
		for (auto itr2 = vecGameObjects.begin(); itr2 != vecGameObjects.end(); ++itr2)
		{
			if (itr2 == itr) // Dont check self
				continue;

			cGameObject* go2 = *itr2;

			switch (go2->collisionShapeType)
			{
				case eCollisionShapeType::AABB:
				{
					switch (go->collisionShapeType)
					{
						case eCollisionShapeType::SPHERE:
						{
							// Find point p on AABB closest to sphere center
							glm::vec3 p = ClosestPtPointAABB(go->position, go2->collisionObjectInfo.minmax->first + go2->position, go2->collisionObjectInfo.minmax->second + go2->position);
							glm::vec3 v = go->position - p;

							//if (debug_mode)
							//{
							//	debugRenderer->addLine(go->position, go2->position, glm::vec3(0.0f, 1.0f, 0.0f), 0);
							//}

							float dvv = dot(v, v);
							float rr = go->collisionObjectInfo.radius * go->collisionObjectInfo.radius;
							if (dvv <= rr)
							{
								float mag = length(go->velocity);
								go->velocity = mag * glm::normalize(v) * go->bounciness;
								go->position += glm::normalize(v) * (go->collisionObjectInfo.radius - length(v));
							}
							break;
						} // AABB - SPHERE
					} // switch go1 shape
					break;
				}
				case eCollisionShapeType::SPHERE:
				{
					float d = distance(go->position, go2->position);
					//if (debug_mode)
					//{
					//	debugRenderer->addLine(go->position, go2->position, glm::vec3(0.0f, 1.0f, 0.0f), 0);
					//}
					if (d < (go->collisionObjectInfo.radius + go2->collisionObjectInfo.radius))
					{
						glm::vec3 collisionVector = go->position - go2->position;
						glm::vec3 normCollisionVector = glm::normalize(collisionVector);
						float overlap = d - (go->collisionObjectInfo.radius + go2->collisionObjectInfo.radius);
						float halflap = -overlap / 2.0f;

						go->velocity = length(go->velocity) * normCollisionVector * go->bounciness;
						go->position += halflap * normCollisionVector;
						if (go2->inverseMass)
						{
							go2->velocity = length(go2->velocity) * -normCollisionVector * go2->bounciness;
							go2->position += halflap * -normCollisionVector;
						}
					}
					break; // SPHERE
				}
				case eCollisionShapeType::MESH:
					switch (go->collisionShapeType)
					{
						case eCollisionShapeType::SPHERE:
						{
							// Sphere x Mesh collision
							glm::vec3 closestPoint = glm::vec3(0.0f, 0.0f, 0.0f);
							float closestDistance = FLT_MAX;
							glm::vec3 p1, p2, p3;
							unsigned closestTriIndex = 0;
							cMesh* mesh = go2->collisionObjectInfo.meshes->second; // transformed collision mesh
							if (!mesh)
								continue;

							for (unsigned i = 0; i < mesh->vecTriangles.size(); ++i)
							{
								p1 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_1].x,
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_1].y,
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_1].z);

								p2 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_2].x,
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_2].y,
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_2].z);

								p3 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_3].x,
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_3].y,
									mesh->vecVertices[mesh->vecTriangles[i].vert_index_3].z);

								glm::vec3 curClosetPoint = ClosestPtPointTriangle(go->position, p1, p2, p3);

								// Is this the closest so far?
								float distanceNow = glm::distance(curClosetPoint, go->position);

								// is this closer than the closest distance
								if (distanceNow <= closestDistance)
								{
									closestDistance = distanceNow;
									closestPoint = curClosetPoint;
									closestTriIndex = i;
								}
							}

							if (debug_mode)
							{
								debugRenderer->addTriangle(
									glm::vec3(
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].x,
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].y,
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].z),
									glm::vec3(
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].x,
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].y,
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].z),
									glm::vec3(
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].x,
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].y,
										mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].z),
									glm::vec3(1.0f, 0.0f, 0.0f), 0);

								debugRenderer->addLine(go->position, closestPoint, glm::vec3(0.0f, 1.0f, 0.0f), 0);
							}


							if (closestDistance <= go->collisionObjectInfo.radius)
							{
								p1 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].x,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].y,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].z);
																		 
								p2 = glm::vec3(							 
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].x,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].y,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].z);
																		 
								p3 = glm::vec3(							 
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].x,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].y,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].z);

								glm::vec3 n1, n2, n3;
								n1 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].nx,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].ny,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_1].nz);

								n2 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].nx,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].ny,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_2].nz);

								n3 = glm::vec3(
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].nx,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].ny,
									mesh->vecVertices[mesh->vecTriangles[closestTriIndex].vert_index_3].nz);

								glm::vec3 faceNorm = CalcNormalOfFace(p1, p2, p3, n1, n2, n3);
								//go->velocity = mag * faceNorm * go->bounciness;
								glm::vec3 refl = glm::normalize(glm::reflect(go->velocity, faceNorm));
								go->velocity = refl * length(go->velocity) * go->bounciness;
								// move outside the tri
								go->position += faceNorm * (go->collisionObjectInfo.radius - closestDistance);
								debugRenderer->addLine(closestPoint, closestPoint + faceNorm, glm::vec3(0.0, 1.0f, 1.0f), 1.0f);
								debugRenderer->addLine(closestPoint, closestPoint + refl, glm::vec3(1.0, 1.0f, 0.0f), 1.0f);
							} // if closestDistance <= radius
							break;
						} // MESH-SPHERE
					} // switch go shape type
				break; // MESH
			} // switch go2 shape type
			
		} // For gameobjects 2
	} // For gameobjects 1
}