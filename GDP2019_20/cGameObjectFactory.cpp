#include "cGameObjectFactory.hpp"

#include "cGameObject.hpp"
#include "cSineWaveMovingGameObject.hpp"
#include "cPlinkoBallGameObject.hpp"

cGameObject* cGameObjectFactory::createFromJSON(Json::Value& obj, std::map<std::string, cMesh*> & mapMeshes)
{
	std::string type = obj["type"].asString();
	if (type == "basic")
	{
		return new cGameObject(obj, mapMeshes);
	}
	if (type == "PlinkoBall")
	{
		return new cPlinkoBallGameObject(obj, mapMeshes);
	}
	else if (type == "SineMovingObject")
	{
		return new cSineWaveMovingGameObject(obj, mapMeshes);
	}
}

cGameObject* cGameObjectFactory::createFromType(std::string type)
{
	if (type == "basic")
	{
		return new cGameObject();
	}
	else if (type == "SineMovingObject")
	{
		return new cSineWaveMovingGameObject();
	}
}

std::string cGameObjectFactory::getTypeString()
{
	return "Basic";
}
