#pragma once
#include <iSlidingBoxComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cSlidingBoxComponent : public iSlidingBoxComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;
		btSliderConstraint* constraint;

	public:
		cSlidingBoxComponent(sSlidingBoxDef def);
		virtual ~cSlidingBoxComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;
	};
}