#pragma once
#include "iPhysicsWorld.h"
#include "iBallComponent.h"
#include "iBoxComponent.h"
#include "iSlidingBoxComponent.h"
#include "iCylinderComponent.h"
#include "iConeComponent.h"
#include "iPlaneComponent.h"
#include "iClothComponent.h"
#include "iCharacterComponent.h"
#include "iSwingingConeComponent.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}
		virtual iPhysicsWorld* CreateWorld() = 0;

		// components
		virtual iBallComponent* CreateBall(const sBallDef& def) = 0;
		virtual iBoxComponent* CreateBox(const sBoxDef& def) = 0;
		virtual iSlidingBoxComponent* CreateSlidingBox(const sSlidingBoxDef& def) = 0;
		virtual iCylinderComponent* CreateCylinder(const sCylinderDef& def) = 0;
		virtual iConeComponent* CreateCone(const sConeDef& def) = 0;
		virtual iSwingingConeComponent* CreateSwingingCone(const sSwingingConeDef& def) = 0;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def) = 0;
		virtual iClothComponent* CreateCloth(const sClothDef& def) = 0;
		virtual iCharacterComponent* CreateCharacter(const sCharacterDef& def) = 0;
	};
}