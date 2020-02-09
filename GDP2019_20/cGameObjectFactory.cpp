#include "cGameObjectFactory.hpp"

#include "cGameObject.hpp"
#include "cPhysicsGameObject.hpp"

iGameObject* cGameObjectFactory::createFromJSON(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	std::string type = obj["type"].asString();
	if (type == "basic")
	{
		return new cGameObject(obj);
	}
	else if (type == "physics")
	{
		return new cPhysicsGameObject(obj);
	}
	return nullptr;
}

iGameObject* cGameObjectFactory::createFromType(std::string type, std::map<std::string, cMesh*>& mapMeshes)
{
	if (type == "basic")
	{
		return new cGameObject();
	}
	return nullptr;
}

std::string cGameObjectFactory::getTypeString()
{
	return "Basic";
}
