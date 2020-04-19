#pragma once
#include <iFreeSwingingCylinderComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cFreeSwingingCylinderComponent : public iFreeSwingingCylinderComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;
		btConeTwistConstraint* constraint;

	public:
		cFreeSwingingCylinderComponent(sFreeSwingingCylinderDef def);
		virtual ~cFreeSwingingCylinderComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;

		// Inherited via iFreeSwingingCylinderComponent
		virtual int getID() override;
		virtual void setID(int id) override;
	};
}