#pragma once

#include "iEngine.hpp"
#include "iWeapon.hpp"

// abstract ship
class iShip
{
public:
	virtual void setEngine(iEngine* engine) = 0;
	virtual void setWeapon(iWeapon* weapon) = 0;
};