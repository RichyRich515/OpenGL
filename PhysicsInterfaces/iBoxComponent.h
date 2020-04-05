#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sBoxDef
	{
		float Mass;
		glm::vec3 Extents;
		glm::vec3 Position;
		float Elasticity;
	};

	class iBoxComponent : public iPhysicsComponent
	{
	public:
		virtual ~iBoxComponent() {}

	protected:
		iBoxComponent() : iPhysicsComponent(ePhysicsComponentType::box) {}

	private:
		iBoxComponent(const iBoxComponent& other) = delete;
		iBoxComponent& operator=(const iBoxComponent& other) = delete;
	};
}