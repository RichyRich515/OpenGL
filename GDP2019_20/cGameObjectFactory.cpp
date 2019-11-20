#include "cGameObjectFactory.hpp"

#include "cGameObject.hpp"
#include "cPelican.hpp"

cGameObject* cGameObjectFactory::createFromJSON(Json::Value& obj, std::map<std::string, cMesh*> & mapMeshes)
{
	std::string type = obj["type"].asString();
	if (type == "basic")
	{
		return new cGameObject(obj, mapMeshes);
	}
	if (type == "pelican")
	{
		return new cPelican(obj, mapMeshes);
	}
	return nullptr;
}

cGameObject* cGameObjectFactory::createFromType(std::string type, std::map<std::string, cMesh*>& mapMeshes)
{
	if (type == "basic")
	{
		return new cGameObject();
	}
}

std::string cGameObjectFactory::getTypeString()
{
	return "Basic";
}
