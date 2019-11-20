#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>

class cParticle
{
public:
	glm::vec3 position;
	glm::quat qOrientation;
	glm::vec3 velocity;
	float scale;
	glm::vec4 color;
	float lifeTime;
};