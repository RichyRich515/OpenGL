#pragma once

#include "cGameObject.hpp"

class cPlinkoBallGameObject : public cGameObject
{
public:
	cPlinkoBallGameObject();
	cPlinkoBallGameObject(std::string name, glm::vec3 startingPos);
	cPlinkoBallGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);

	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value& obj);
private:
	glm::vec3 startingPos;
};