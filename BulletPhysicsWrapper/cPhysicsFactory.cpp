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
#include "cTriggerSphereComponent.hpp"
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
		auto component = new cBallComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iBoxComponent* cPhysicsFactory::CreateBox(const sBoxDef& def)
	{
		auto component = new cBoxComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iSlidingBoxComponent* cPhysicsFactory::CreateSlidingBox(const sSlidingBoxDef& def)
	{
		auto component = new cSlidingBoxComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iCylinderComponent* cPhysicsFactory::CreateCylinder(const sCylinderDef& def)
	{
		auto component = new cCylinderComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iFreeSwingingCylinderComponent* cPhysicsFactory::CreateFreeSwingingCylinder(const sFreeSwingingCylinderDef& def)
	{
		auto component = new cFreeSwingingCylinderComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iConeComponent* cPhysicsFactory::CreateCone(const sConeDef& def)
	{
		auto component = new cConeComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iSwingingConeComponent* cPhysicsFactory::CreateSwingingCone(const sSwingingConeDef& def)
	{
		auto component = new cSwingingConeComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(const sPlaneDef& def)
	{
		auto component = new cPlaneComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iClothComponent* cPhysicsFactory::CreateCloth(const sClothDef& def)
	{
		return nullptr;
	}

	iTriggerSphereComponent* cPhysicsFactory::CreateTriggerSphere(const sTriggerSphereDef& def)
	{
		auto component = new cTriggerSphereComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}

	iCharacterComponent* cPhysicsFactory::CreateCharacter(const sCharacterDef& def)
	{
		auto component = new cCharacterComponent(def);
		component->setID(next_id);
		++next_id;
		return component;
	}
}

nPhysics::iPhysicsFactory* MakePhysicsFactory()
{
	return new nPhysics::cPhysicsFactory();
}
