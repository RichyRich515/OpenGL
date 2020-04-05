#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sCylinderDef
	{
		float Mass;
		glm::vec3 Extents;
		glm::vec3 Position;
		float Elasticity;
	};

	class iCylinderComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCylinderComponent() {}

	protected:
		iCylinderComponent() : iPhysicsComponent(ePhysicsComponentType::cylinder) {}

	private:
		iCylinderComponent(const iCylinderComponent& other) = delete;
		iCylinderComponent& operator=(const iCylinderComponent& other) = delete;
	};
}