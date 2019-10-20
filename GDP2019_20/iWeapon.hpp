#pragma once

#include <json/json.h>

class iWeapon
{
public:
	virtual float getDamage() = 0;
	virtual float getFireSpeed() = 0;
	virtual float increaseAmmo() = 0;
	virtual float decreaseAmmo() = 0;
	
	virtual void deserialize(Json::Value& obj) = 0;
	virtual Json::Value serialize() = 0;
};