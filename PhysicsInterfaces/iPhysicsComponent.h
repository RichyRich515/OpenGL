#pragma once
#include <glm/mat4x4.hpp>
#include "eComponentType.h"

namespace nPhysics
{
	class iPhysicsComponent;
	typedef void(*OnCollisionFunctionType)(iPhysicsComponent*);

	class iPhysicsComponent
	{
	public:
		virtual ~iPhysicsComponent() {}
		inline const ePhysicsComponentType& GetComponentType() { return mComponentType; }

		// IDs are created by the factory automatically
		virtual int getID() = 0;

		// Shouldn't use this unless you have a good reason
		virtual void setID(int id) = 0;

		virtual void GetTransform(glm::mat4& transformOut) = 0;

		virtual void ApplyForce(const glm::vec3& force) = 0;

		virtual void OnCollision(iPhysicsComponent* other)
		{
			if (this->mOnCollisionFunction)
			{
				this->mOnCollisionFunction(other);
			}
		}

		virtual void SetOnCollisionFunction(OnCollisionFunctionType func)
		{
			this->mOnCollisionFunction = func;
		}

	protected:
		iPhysicsComponent(ePhysicsComponentType componentType)
			: mComponentType(componentType) {}

	private:
		ePhysicsComponentType mComponentType;
		OnCollisionFunctionType mOnCollisionFunction;

		// get rid of these!
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;
	};
}