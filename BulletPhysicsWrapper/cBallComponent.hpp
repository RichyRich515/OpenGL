#pragma once
#include <iBallComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cBallComponent : public iBallComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;

	public:
		cBallComponent(sBallDef def);
		virtual ~cBallComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;

		virtual int getID() override;
		virtual void setID(int id) override;
	};
}