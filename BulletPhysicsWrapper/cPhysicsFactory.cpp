#include "cPhysicsFactory.hpp"
#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cBoxComponent.hpp"
#include "cCylinderComponent.hpp"
#include "cConeComponent.hpp"
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

	iBoxComponent* cPhysicsFactory::CreateBox(const sBoxDef& def)
	{
		return new cBoxComponent(def);
	}

	iCylinderComponent* cPhysicsFactory::CreateCylinder(const sCylinderDef& def)
	{
		return new cCylinderComponent(def);
	}

	iConeComponent* cPhysicsFactory::CreateCone(const sConeDef& def)
	{
		return new cConeComponent(def);
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
