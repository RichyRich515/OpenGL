#pragma once

#include <json/json.h>

#include "cGameObject.hpp"

// Abstract factory
class iGameObjectFactory
{
public:
	virtual cGameObject* createFromJSON(Json::Value obj) = 0;
	virtual cGameObject* createFromType(std::string type) = 0;
};