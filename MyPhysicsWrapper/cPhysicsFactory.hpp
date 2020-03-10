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
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def) override;
		virtual iClothComponent* CreateCloth(const sClothDef& def) override;
	};
}