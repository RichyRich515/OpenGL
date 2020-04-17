#pragma once
#include <iPhysicsWorld.h>
#include <btBulletDynamicsCommon.h>
#include "bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	private:
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* broadphase;
		btGhostPairCallback* ghostPairCallback;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();

		virtual void Update(float dt) override;
		virtual bool AddComponent(iPhysicsComponent* component) override;
		virtual bool RemoveComponent(iPhysicsComponent* component) override;
	};
}