#pragma once

#include <iCylinderComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cCylinderComponent : public iCylinderComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;

	public:
		cCylinderComponent(sCylinderDef def);
		virtual ~cCylinderComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;
	};
}