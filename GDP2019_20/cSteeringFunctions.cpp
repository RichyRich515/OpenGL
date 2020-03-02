#include "cSteeringFunctions.hpp"

#include "GLCommon.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

glm::vec3 cSteeringFunctions::steerSeekArrive(glm::vec3 const& pos, glm::vec3 const& dest, glm::vec3 const& curvel, float const& radius, float const& maxSpeed, float const& dt)
{
	glm::vec3 desiredVelocity = dest - pos;

	float dist = glm::length(desiredVelocity);

	desiredVelocity = glm::normalize(desiredVelocity);

	if (dist < radius)
	{
		desiredVelocity = desiredVelocity * maxSpeed * (dist / radius);
	}
	else
	{
		desiredVelocity *= maxSpeed;
	}

	glm::vec3 steer = desiredVelocity - curvel;

	if (glm::length(curvel + steer) > maxSpeed)
	{
		if (glm::length(steer) == 0)
		{
			steer = curvel;
		}
		else
		{
			steer = glm::normalize(steer) * maxSpeed;
		}
	}

	return steer;
}

glm::vec3 cSteeringFunctions::steerFlee(glm::vec3 const& pos, glm::vec3 const& dest, glm::vec3 const& curvel, float const& maxSpeed, float const& dt)
{
	return -steerSeekArrive(pos, dest, curvel, 0.0f, maxSpeed, dt);
}

glm::vec3 cSteeringFunctions::steerPursue(glm::vec3 const& pos, glm::vec3 const& dest, glm::vec3 const& destVelocity, glm::vec3 const& curvel, float const& maxSpeed, float const& dt)
{
	//calculate the number of frames we are looking ahead
	glm::vec3 dist = dest - pos;
	float T = glm::length(dist) / maxSpeed;

	//the future target point the vehicle will pursue towards
	glm::vec3 futureDest = dest + (destVelocity * T);

	return steerSeekArrive(pos, futureDest, curvel, 0.0f, maxSpeed, dt);
}

glm::vec3 cSteeringFunctions::steerWander(glm::vec3 const& pos, glm::vec3 const& curvel, float const& dist, float const& radius, float const& maxSpeed, float const& dt)
{
	float angle = ((rand() % RAND_MAX) / (float)RAND_MAX * 360.0f) * glm::pi<float>() / 180.0f;
	float xOffset = sinf(angle);
	float zOffset = cosf(angle);
	glm::vec3 offset = glm::vec3(xOffset, 0.0f, zOffset) * radius;
	glm::vec3 dest = pos;
	if (glm::length(curvel) == 0.0f)
	{
		dest += offset;
	}
	else
	{
		dest += glm::normalize(curvel) * dist + offset;
	}

	return steerSeekArrive(pos, dest, curvel, 0, maxSpeed, dt);
}


glm::vec3 cSteeringFunctions::steerAvoid(glm::vec3 const& pos, glm::vec3 const& curvel, std::vector<cGameObject*> const& threats, float const& threat_radius, float const& lineOfSightRange, float const& maxSpeed, float const& dt)
{
	//calculate vectors to search ahead for possible objects
	glm::vec3 lineOfSight = pos + glm::normalize(curvel) * lineOfSightRange;
	glm::vec3 lineOfSightHalf = lineOfSight * 0.5f;
	glm::vec3 threatPos(FLT_MAX);
	bool isThreatDetected = false;
	for (auto go : threats)
	{
		bool bIsCollision = glm::distance(go->getPosition(), lineOfSight) < threat_radius || glm::distance(go->getPosition(), lineOfSightHalf) < threat_radius;
		//grab the obstacle closest to our vehicle
		if (bIsCollision && glm::distance(pos, go->getPosition()) < glm::distance(pos, threatPos))
		{
			// get closest threat
			threatPos = go->getPosition();
			isThreatDetected = true;
		}
	}

	if (isThreatDetected)
		return glm::normalize(lineOfSight - threatPos) * maxSpeed;

	return glm::vec3(0.0);
}