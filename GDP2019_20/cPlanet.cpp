#include "cPlanet.hpp"

cPlanet::cPlanet()
{
	this->fuel = 10.0f;
}

cPlanet::cPlanet(std::string name)
{
	this->name = name;
	this->fuel = 10.0f;
}

cPlanet::cPlanet(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj, mapMeshes);
	this->instatiateUniqueVariables(obj);
}

void cPlanet::init()
{

}

void cPlanet::update(float dt)
{

}

sMessage cPlanet::message(sMessage const& msg)
{
	if (msg.name == "requestfuel")
		return sMessage("refuel", msg.data.f);
	else
		return sMessage();
}

void cPlanet::instatiateUniqueVariables(Json::Value& obj)
{
	fuel = obj["uniques"]["fuel"].asFloat();
}

void cPlanet::serializeUniqueVariables(Json::Value& obj)
{
	obj["uniques"]["fuel"] = fuel;
}
