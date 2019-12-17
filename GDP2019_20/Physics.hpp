// Physics.hpp
// Physics stuff
// Richard Woods
// 2019-09-19
#pragma once

#include <vector>
#include "cGameObject.hpp"
#include "DebugRenderer/cDebugRenderer.h"

constexpr float MAX_PHYSICS_DELTA_TIME = 0.017f;

struct sClosestTriInfo
{
	unsigned triIndex;
	glm::vec3 point;
	float distance;
};

bool CheckIfPointInMeshAndRadius(cMesh* mesh, glm::vec3 const& point, float r);
glm::vec3 CalcNormalOfFace(glm::vec3 const& v1, glm::vec3 const& v2, glm::vec3 const& v3, glm::vec3 const& n1, glm::vec3 const& n2, glm::vec3 const& n3);
sClosestTriInfo findClosestTriToPoint(cMesh* mesh, glm::vec3 point);
int TestPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);


void physicsUpdate(std::vector<cGameObject*>& vecGameObjects, glm::vec3 gravity, float dt, cDebugRenderer* debugRenderer, bool debug_mode);