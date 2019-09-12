#pragma once

#include <glm/vec3.hpp>

#include <string>

class cGameObject
{
	std::string meshName;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
};