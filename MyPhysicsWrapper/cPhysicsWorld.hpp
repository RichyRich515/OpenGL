#pragma once
#include <iPhysicsWorld.h>
#include <cWorld.h>

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	private:
		phys::cWorld* world;
	public:
		cPhysicsWorld();

		virtual void Update(float dt) override;
		virtual bool AddComponent(iPhysicsComponent* component) override;
		virtual bool RemoveComponent(iPhysicsComponent* component) override;
	};
}