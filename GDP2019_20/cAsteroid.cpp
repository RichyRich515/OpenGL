#include "cAsteroid.hpp"

cAsteroid::cAsteroid()
{
	this->fuel = 10.0f;
}

cAsteroid::cAsteroid(std::string name)
{
	this->name = name;
	this->fuel = 10.0f;
}

cAsteroid::cAsteroid(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj, mapMeshes);
	this->instatiateUniqueVariables(obj);
}

void cAsteroid::init()
{

}

void cAsteroid::update(float dt)
{

}

sMessage cAsteroid::message(sMessage const& msg)
{
	if (msg.name == "requestfuel")
		return sMessage("refuel", 1.0f);
	else
		return sMessage();
}

void cAsteroid::instatiateUniqueVariables(Json::Value& obj)
{
	fuel = obj["uniques"]["fuel"].asFloat();
}

void cAsteroid::serializeUniqueVariables(Json::Value& obj)
{
	obj["uniques"]["fuel"] = fuel;
}
