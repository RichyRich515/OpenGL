#pragma once

#include <string>
#include <map>

#include <json/json.h>
#include "cGameObject.hpp"

// Abstract factory
class iGameObjectFactory
{
public:
	virtual cGameObject* createFromJSON(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes) = 0;
	virtual cGameObject* createFromType(std::string type) = 0;
	virtual std::string getTypeString() = 0;
};