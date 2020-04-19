#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sTriggerSphereDef
	{
		float Radius;
		glm::vec3 Position;
	};

	class iTriggerSphereComponent : public iPhysicsComponent
	{
	public:
		virtual ~iTriggerSphereComponent() {}

		virtual bool IsTriggeredBy(int id) = 0;

	protected:
		iTriggerSphereComponent() : iPhysicsComponent(ePhysicsComponentType::trigger_sphere) {}

	private:
		iTriggerSphereComponent(const iTriggerSphereComponent& other) = delete;
		iTriggerSphereComponent& operator=(const iTriggerSphereComponent& other) = delete;
	};
}