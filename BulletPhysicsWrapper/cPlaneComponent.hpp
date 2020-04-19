#pragma once
#include <iPlaneComponent.h>
#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cPlaneComponent : public iPlaneComponent
	{
	private:
		friend class cPhysicsWorld;

		btRigidBody* body;

	public:
		cPlaneComponent(sPlaneDef def);
		virtual ~cPlaneComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		// Inherited via iPlaneComponent
		virtual void ApplyForce(const glm::vec3& force) override;

		// Inherited via iPlaneComponent
		virtual int getID() override;
		virtual void setID(int id) override;
	};
}