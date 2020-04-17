#pragma once
#include <iCharacterComponent.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics\Character\btKinematicCharacterController.h>

namespace nPhysics
{
	class cCharacterComponent : public iCharacterComponent
	{
	private:
		friend class cPhysicsWorld;

		btKinematicCharacterController* character;
		btPairCachingGhostObject* ghostObject;

	public:
		cCharacterComponent(sCharacterDef def);
		virtual ~cCharacterComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;

		// Inherited via iCharacterComponent
		virtual void Jump() override;
		virtual void Move(glm::vec3 v, float time) override;
		virtual bool CanJump() override;
		virtual void GetVelocity(glm::vec3& v) override;
	};
}