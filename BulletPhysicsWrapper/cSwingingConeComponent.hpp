#pragma once
#include <iSwingingConeComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cSwingingConeComponent : public iSwingingConeComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;
		btHingeConstraint* constraint;

	public:
		cSwingingConeComponent(sSwingingConeDef def);
		virtual ~cSwingingConeComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;
	};
}