#include "cBasicEngine.hpp"

cBasicEngine::cBasicEngine()
{
	this->_speed = 1.0f;
	this->_fuel = 10.0f;
	this->_fuelBurn = 0.5f; // per second
}

float cBasicEngine::getSpeed()
{
	return this->_speed;
}

float cBasicEngine::getFuel()
{
	return this->_fuel;
}

float cBasicEngine::increaseFuel(float f)
{
	this->_fuel += f;
	return this->_fuel;
}

float cBasicEngine::consumeFuel(float dt)
{
	this->_fuel -= this->_fuelBurn * dt;
	return this->_fuel;
}

void cBasicEngine::deserialize(Json::Value& obj)
{
	this->_speed = obj["speed"].asFloat();
	this->_fuel = obj["fuel"].asFloat();
	this->_fuelBurn = obj["fuelBurn"].asFloat();
}

Json::Value cBasicEngine::serialize()
{
	Json::Value obj = Json::objectValue;
	obj["speed"] = this->_speed;
	obj["fuel"] = this->_fuel;
	obj["fuelBurn"] = this->_fuelBurn;
	return obj;
}
