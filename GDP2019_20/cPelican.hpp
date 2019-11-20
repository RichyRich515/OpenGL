#pragma once

#include "cGameObject.hpp"

class cPelican : public cGameObject
{
public:
	cPelican();
	cPelican(std::string name, glm::vec3 startingPos);
	cPelican(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);

	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value& obj);

	float forwardAccel;
	float forwardDecel;
	float updownAccel;
	float updownDecel;
	float leftrightAccel;
	float leftrightDecel;
	float rollAccel;
	float rollDecel;
	
	float maxSpeed;
	float speed;


	float maxRotationSpeed;
	float YrotationSpeed;
	float XrotationSpeed;
	float ZrotationSpeed;
};