#pragma once

#include "cBasicShip.hpp"

class cRefuelShip : public cBasicShip
{
public:
	cRefuelShip();
	cRefuelShip(std::string name);
	cRefuelShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual ~cRefuelShip();
	// Inherited via cGameObject
	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value& obj);

protected:
	float fuel_reservoir;
};