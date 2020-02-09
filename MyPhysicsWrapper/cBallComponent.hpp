#pragma once

#include <iBallComponent.h>
#include <cRigidBody.h>
#include <shapes.h>

namespace nPhysics
{
	class cBallComponent : public iBallComponent
	{
	private:
		friend class cPhysicsWorld;

		phys::cRigidBody* body;
		phys::cSphere* shape;

	public:
		cBallComponent(sBallDef def);
		virtual ~cBallComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;
	};
}