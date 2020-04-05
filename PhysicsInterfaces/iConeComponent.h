#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sConeDef
	{
		float Mass;
		float Radius;
		float Height;
		glm::vec3 Position;
		float Elasticity;
	};

	class iConeComponent : public iPhysicsComponent
	{
	public:
		virtual ~iConeComponent() {}

	protected:
		iConeComponent() : iPhysicsComponent(ePhysicsComponentType::cone) {}

	private:
		iConeComponent(const iConeComponent& other) = delete;
		iConeComponent& operator=(const iConeComponent& other) = delete;
	};
}