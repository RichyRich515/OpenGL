#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sCharacterDef
	{
		float Mass;
		float Radius;
		float Height;
		glm::vec3 Position;
		float JumpSpeed;
	};

	class iCharacterComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCharacterComponent() {}

		virtual void Jump() = 0;
		virtual bool CanJump() = 0;

		virtual void Move(glm::vec3 v, float time) = 0;

		virtual void GetVelocity(glm::vec3 &v) = 0;

	protected:
		iCharacterComponent() : iPhysicsComponent(ePhysicsComponentType::character) {}

	private:
		iCharacterComponent(const iCharacterComponent& other) = delete;
		iCharacterComponent& operator=(const iCharacterComponent& other) = delete;
	};
}