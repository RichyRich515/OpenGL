#pragma once

#include "cGameObject.hpp"

class cAsteroid : public cGameObject
{
public:
	cAsteroid();
	cAsteroid(std::string name);
	cAsteroid(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);

	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value& obj);
private:
	float fuel;
};