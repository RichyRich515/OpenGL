#pragma once
#include <iConeComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cConeComponent : public iConeComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;

	public:
		cConeComponent(sConeDef def);
		virtual ~cConeComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;
	};
}