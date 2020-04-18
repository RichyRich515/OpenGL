#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sFreeSwingingCylinderDef
	{
		float Mass;
		glm::vec3 Extents;
		glm::vec3 Position;
		float Elasticity;
	};

	class iFreeSwingingCylinderComponent : public iPhysicsComponent
	{
	public:
		virtual ~iFreeSwingingCylinderComponent() {}

	protected:
		iFreeSwingingCylinderComponent() : iPhysicsComponent(ePhysicsComponentType::free_swinging_cylinder) {}

	private:
		iFreeSwingingCylinderComponent(const iFreeSwingingCylinderComponent& other) = delete;
		iFreeSwingingCylinderComponent& operator=(const iFreeSwingingCylinderComponent& other) = delete;
	};
}