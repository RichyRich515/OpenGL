#pragma once

#include <iPlaneComponent.h>
#include <cRigidBody.h>


namespace nPhysics
{
	class cPlaneComponent : public iPlaneComponent
	{
	private:
		friend class cPhysicsWorld;
		phys::cRigidBody* body;

	public:
		virtual ~cPlaneComponent() {}
		virtual void GetTransform(glm::mat4& transformOut) override;
	};
}