#pragma once
#include "bullet/btBulletDynamicsCommon.h"
#include <iTriggerSphereComponent.h>
#include "bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cTriggerSphereComponent : public iTriggerSphereComponent
	{
	private:
		friend class cPhysicsWorld;

		btPairCachingGhostObject* ghostObject;

	public:
		cTriggerSphereComponent(sTriggerSphereDef);
		virtual ~cTriggerSphereComponent();

		virtual bool IsTriggeredBy(int id) override;

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(glm::vec3 const& v) override;

		virtual int getID() override;
		virtual void setID(int id) override;
	};
}