#include "cSpaceGameFactory.hpp"

#include "cShipBuilder.hpp"
#include "cBasicShip.hpp"
#include "cPlanet.hpp"

cGameObject* cSpaceGameFactory::createFromJSON(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	std::string type = obj["type"].asString();
	if (type == "basicShip")
	{
		cBasicShip* ship = cShipBuilder::MakeBasicShip(obj, mapMeshes);
		ship->setEngine(cShipBuilder::MakeBasicEngine(obj["uniques"]["engine"]));
		ship->setWeapon(cShipBuilder::MakeBasicWeapon(obj["uniques"]["weapon"]));
		return ship;
	}
	else if (type == "refuelShip")
	{
		cBasicShip* ship = cShipBuilder::MakeRefuelShip(obj, mapMeshes);
		ship->setEngine(cShipBuilder::MakeBasicEngine(obj["uniques"]["engine"]));
		ship->setWeapon(cShipBuilder::MakeBasicWeapon(obj["uniques"]["weapon"]));
		return ship;
	}
	else if (type == "planet")
	{
		return new cPlanet(obj, mapMeshes);
	}
	else if (type == "asteroid")
	{
		return new cPlanet(obj, mapMeshes);
	}
	else
	{
		return nullptr;
	}
}

cGameObject* cSpaceGameFactory::createFromType(std::string type)
{
	// TODO: this
	return nullptr;
}

std::string cSpaceGameFactory::getTypeString()
{
	return "SpaceGame";
}
