#include "cPhysicsFactory.hpp"
#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"

namespace nPhysics
{
	iPhysicsWorld* cPhysicsFactory::CreateWorld()
	{
		return new cPhysicsWorld();
	}

	iBallComponent* cPhysicsFactory::CreateBall(const sBallDef& def)
	{
		return new cBallComponent(def);
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(const sPlaneDef& def)
	{
		return new cPlaneComponent(def);
	}
}