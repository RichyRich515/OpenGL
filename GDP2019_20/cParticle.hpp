#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class cParticle
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	float lifeTime;
};