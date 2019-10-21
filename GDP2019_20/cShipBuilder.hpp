#pragma once

#include <json/json.h>

#include "cBasicShip.hpp"
#include "cRefuelShip.hpp"
#include "cBasicEngine.hpp"
#include "cBasicWeapon.hpp"

class cShipBuilder
{
public:
	static cBasicShip* MakeBasicShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	static cBasicShip* MakeRefuelShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	static iEngine* MakeBasicEngine(Json::Value& obj);
	static iWeapon* MakeBasicWeapon(Json::Value& obj);
};