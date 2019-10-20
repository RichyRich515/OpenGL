#pragma once

#include "iEngine.hpp"

class cBasicEngine : public iEngine
{
public:
	cBasicEngine();

	// Inherited via iEngine
	virtual float getSpeed() override;
	virtual float getFuel() override;

	virtual float increaseFuel(float f) override;
	virtual float consumeFuel(float f) override;

	virtual void deserialize(Json::Value& obj) override;
	virtual Json::Value serialize() override;
private:
	float _speed;
	float _fuel;
	float _fuelBurn;
};