#pragma once

#include <iPlaneComponent.h>
#include <cRigidBody.h>
#include <shapes.h>



namespace nPhysics
{
	class cPlaneComponent : public iPlaneComponent
	{
	private:
		friend class cPhysicsWorld;
		phys::cRigidBody* body;
		phys::cPlane* shape;

	public:
		cPlaneComponent(sPlaneDef def);
		virtual ~cPlaneComponent() {}
		virtual void GetTransform(glm::mat4& transformOut) override;
	};
}