#pragma once
#include <iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	private:

	public:
		virtual iPhysicsWorld* CreateWorld() override;
		virtual iBallComponent* CreateBall(const sBallDef& def) override;
		virtual iBoxComponent* CreateBox(const sBoxDef& def) override;
		virtual iCylinderComponent* CreateCylinder(const sCylinderDef& def) override;
		virtual iConeComponent* CreateCone(const sConeDef& def) override;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def) override;
		virtual iClothComponent* CreateCloth(const sClothDef& def) override;
		virtual iCharacterComponent* CreateCharacter(const sCharacterDef& def) override;
	};
}