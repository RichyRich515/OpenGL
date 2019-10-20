#pragma once

#include "iWeapon.hpp"

class cBasicWeapon : public iWeapon
{
public:
	// Inherited via iWeapon
	virtual float getDamage() override;
	virtual float getFireSpeed() override;
	virtual float increaseAmmo() override;
	virtual float decreaseAmmo() override;

	virtual void deserialize(Json::Value& obj) override;
	virtual Json::Value serialize() override;
private:
	float damage;
	float fireSpeed;
	float ammo;
};