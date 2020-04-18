#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sSlidingBoxDef
	{
		float Mass;
		glm::vec3 Extents;
		glm::vec3 Position;
		float Elasticity;
	};

	class iSlidingBoxComponent : public iPhysicsComponent
	{
	public:
		virtual ~iSlidingBoxComponent() {}

	protected:
		iSlidingBoxComponent() : iPhysicsComponent(ePhysicsComponentType::sliding_box) {}

	private:
		iSlidingBoxComponent(const iSlidingBoxComponent& other) = delete;
		iSlidingBoxComponent& operator=(const iSlidingBoxComponent& other) = delete;
	};
}