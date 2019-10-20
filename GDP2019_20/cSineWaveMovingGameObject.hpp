#pragma once

#include <string>

#include <json/json.h>
#include <glm/vec3.hpp>

#include "cMesh.hpp"
#include "cGameObject.hpp"

class cSineWaveMovingGameObject : public cGameObject
{
public:
	cSineWaveMovingGameObject();
	cSineWaveMovingGameObject(std::string name);
	cSineWaveMovingGameObject(Json::Value& obj, std::map<std::string, cMesh*> & mapMeshes);

	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value & obj);
private:
	float counter;
	glm::vec3 startingPos;
	bool moving;
};