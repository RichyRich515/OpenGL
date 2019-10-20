#include "cBasicWeapon.hpp"

float cBasicWeapon::getDamage()
{
	return 0.0f;
}

float cBasicWeapon::getFireSpeed()
{
	return 0.0f;
}

float cBasicWeapon::increaseAmmo()
{
	return 0.0f;
}

float cBasicWeapon::decreaseAmmo()
{
	return 0.0f;
}

void cBasicWeapon::deserialize(Json::Value& obj)
{
	this->ammo = obj["ammo"].asFloat();
	this->fireSpeed = obj["ammo"].asFloat();
	this->damage = obj["damage"].asFloat();
}

Json::Value cBasicWeapon::serialize()
{
	Json::Value obj = Json::objectValue;
	obj["ammo"] = this->ammo;
	obj["fireSpeed"] = this->fireSpeed;
	obj["damage"] = this->damage;
	return obj;
}
