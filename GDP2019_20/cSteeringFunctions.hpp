#pragma once

#include <glm/vec3.hpp>
#include "cGameObject.hpp"
#include <vector>

class cSteeringFunctions
{
public:
	static glm::vec3 steerSeekArrive(glm::vec3 const& pos, glm::vec3 const& dest, glm::vec3 const& curvel, float const& radius, float const& maxSpeed, float const& dt);
	static glm::vec3 steerFlee(glm::vec3 const& pos, glm::vec3 const& dest, glm::vec3 const& curvel, float const& maxSpeed, float const& dt);
	static glm::vec3 steerPursue(glm::vec3 const& pos, glm::vec3 const& dest, glm::vec3 const& destVelocity, glm::vec3 const& curvel, float const& maxSpeed, float const& dt);
	static glm::vec3 steerWander(glm::vec3 const& pos, glm::vec3 const& curvel, float const& dist, float const& radius, float const& maxSpeed, float const& dt);
	static glm::vec3 steerAvoid(glm::vec3 const& pos, glm::vec3 const& curvel, std::vector<cGameObject*> const& threats, float const& threat_radius, float const& lineOfSightRange, float const& maxSpeed, float const& dt);
};