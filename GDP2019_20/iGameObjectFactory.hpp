#pragma once

#include <string>
#include <map>

#include <json/json.h>
#include "cGameObject.hpp"

// Abstract factory
class iGameObjectFactory
{
public:
	virtual iGameObject* createFromJSON(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes) = 0;
	virtual iGameObject* createFromType(std::string type, std::map<std::string, cMesh*>& mapMeshes) = 0;
	virtual std::string getTypeString() = 0;
};