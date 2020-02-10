#pragma once
#include <glm/mat4x4.hpp>
#include "eComponentType.h"

namespace nPhysics
{
	class iPhysicsComponent
	{
	public:
		virtual ~iPhysicsComponent() {}
		inline const ePhysicsComponentType& GetComponentType() { return mComponentType; }

		virtual void GetTransform(glm::mat4& transformOut) = 0;

		virtual void ApplyForce(const glm::vec3& force) = 0;

	protected:
		iPhysicsComponent(ePhysicsComponentType componentType)
			: mComponentType(componentType) {}
	private:
		ePhysicsComponentType mComponentType;

		// get rid of these!
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;
	};
}