#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sBallDef
	{
		float Mass = 1.0f;
		float Radius = 1.0f;
		glm::vec3 Position = glm::vec3(0.0f);
		float Elasticity = 0.5f;
		glm::vec3 InitialVelocity = glm::vec3(0.0f);
	};

	class iBallComponent : public iPhysicsComponent
	{
	public:
		virtual ~iBallComponent() {}
		
	protected:
		iBallComponent() : iPhysicsComponent(ePhysicsComponentType::ball) {}

	private:
		iBallComponent(const iBallComponent& other) = delete;
		iBallComponent& operator=(const iBallComponent& other) = delete;
	};
}