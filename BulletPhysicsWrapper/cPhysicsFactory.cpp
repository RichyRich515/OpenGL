#include "cPhysicsFactory.hpp"
#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cBoxComponent.hpp"
#include "cSlidingBoxComponent.hpp"
#include "cCylinderComponent.hpp"
#include "cFreeSwingingCylinderComponent.hpp"
#include "cConeComponent.hpp"
#include "cSwingingConeComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cClothComponent.hpp"
#include "cCharacterComponent.hpp"

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

	iSlidingBoxComponent* cPhysicsFactory::CreateSlidingBox(const sSlidingBoxDef& def)
	{
		return new cSlidingBoxComponent(def);
	}

	iCylinderComponent* cPhysicsFactory::CreateCylinder(const sCylinderDef& def)
	{
		return new cCylinderComponent(def);
	}

	iFreeSwingingCylinderComponent* cPhysicsFactory::CreateFreeSwingingCylinder(const sFreeSwingingCylinderDef& def)
	{
		return new cFreeSwingingCylinderComponent(def);
	}

	iConeComponent* cPhysicsFactory::CreateCone(const sConeDef& def)
	{
		return new cConeComponent(def);
	}

	iSwingingConeComponent* cPhysicsFactory::CreateSwingingCone(const sSwingingConeDef& def)
	{
		return new cSwingingConeComponent(def);
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(const sPlaneDef& def)
	{
		return new cPlaneComponent(def);
	}

	iClothComponent* cPhysicsFactory::CreateCloth(const sClothDef& def)
	{
		return new cClothComponent(def);
	}

	iCharacterComponent* cPhysicsFactory::CreateCharacter(const sCharacterDef& def)
	{
		return new cCharacterComponent(def);
	}
}

nPhysics::iPhysicsFactory* MakePhysicsFactory()
{
	return new nPhysics::cPhysicsFactory();
}
