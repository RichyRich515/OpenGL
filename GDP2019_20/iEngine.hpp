#pragma once

#include <json/json.h>

class iEngine
{
public:
	virtual float getSpeed() = 0;
	virtual float getFuel() = 0;
	virtual float increaseFuel(float f) = 0;
	virtual float consumeFuel(float f) = 0;

	virtual void deserialize(Json::Value& obj) = 0;
	virtual Json::Value serialize() = 0;
};