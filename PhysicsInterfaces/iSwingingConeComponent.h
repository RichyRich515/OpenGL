#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sSwingingConeDef
	{
		float Mass;
		float Radius;
		float Height;
		glm::vec3 Position;
		float Elasticity;
	};

	class iSwingingConeComponent : public iPhysicsComponent
	{
	public:
		virtual ~iSwingingConeComponent() {}

	protected:
		iSwingingConeComponent() : iPhysicsComponent(ePhysicsComponentType::swinging_cone) {}

	private:
		iSwingingConeComponent(const iSwingingConeComponent& other) = delete;
		iSwingingConeComponent& operator=(const iSwingingConeComponent& other) = delete;
	};
}