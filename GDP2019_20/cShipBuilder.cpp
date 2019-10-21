#include "cShipBuilder.hpp"

cBasicShip* cShipBuilder::MakeBasicShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	return new cBasicShip(obj, mapMeshes);
}

cBasicShip* cShipBuilder::MakeRefuelShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	return new cRefuelShip(obj, mapMeshes);
}

iEngine* cShipBuilder::MakeBasicEngine(Json::Value& obj)
{
	iEngine* eng = new cBasicEngine();
	eng->deserialize(obj);
	return eng;
}

iWeapon* cShipBuilder::MakeBasicWeapon(Json::Value& obj)
{
	iWeapon* wep = new cBasicWeapon();
	wep->deserialize(obj);
	return wep;
}
