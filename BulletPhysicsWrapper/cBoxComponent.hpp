#pragma once

#include <iBoxComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cBoxComponent : public iBoxComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;

	public:
		cBoxComponent(sBoxDef def);
		virtual ~cBoxComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;
	};
}