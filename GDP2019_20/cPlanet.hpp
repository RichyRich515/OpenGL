#pragma once

#include "cGameObject.hpp"

class cPlanet : public cGameObject
{
public:
	cPlanet();
	cPlanet(std::string name);
	cPlanet(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);

	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value& obj);
private:
	float fuel;
};