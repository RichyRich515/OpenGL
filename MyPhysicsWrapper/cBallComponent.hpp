#pragma once

#include <iBallComponent.h>
#include <cRigidBody.h>
#include <shapes.h>

namespace nPhysics
{
	class cBallComponent : public iBallComponent
	{
		friend class cPhysicsWorld;
	private:
		phys::cRigidBody* body;

		// TODO: dont handle own shape?
		phys::cSphere* shape;

	public:
		cBallComponent(sBallDef def);
		// Inherited via iBallComponent
		virtual void GetTransform(glm::mat4& transformOut) override;
	};
}