#include "cGameObjectFactory.hpp"

#include "cGameObject.hpp"

cGameObject* cGameObjectFactory::createFromJSON(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	std::string type = obj["type"].asString();
	if (type == "basic")
	{
		return new cGameObject(obj);
	}
	return nullptr;
}

cGameObject* cGameObjectFactory::createFromType(std::string type, std::map<std::string, cMesh*>& mapMeshes)
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
