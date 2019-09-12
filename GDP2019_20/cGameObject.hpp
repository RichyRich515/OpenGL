#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>

class cGameObject
{
public:
	std::string meshName;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
	glm::vec4 color;
};