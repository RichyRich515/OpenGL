#include "cPhysicsFactory.hpp"
#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cClothComponent.hpp"

// Export function from DLL
extern "C" __declspec(dllexport) nPhysics::iPhysicsFactory* MakePhysicsFactory();

namespace nPhysics
{
	iPhysicsWorld* cPhysicsFactory::CreateWorld()
	{
		// TODO: def for gravity, RK4, euler and stuff?
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

	iClothComponent* cPhysicsFactory::CreateCloth(const sClothDef& def)
	{
		return new cClothComponent(def);
	}
}

nPhysics::iPhysicsFactory* MakePhysicsFactory()
{
	return new nPhysics::cPhysicsFactory();
}
