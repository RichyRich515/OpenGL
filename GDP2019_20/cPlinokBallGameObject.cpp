#include "cPlinkoBallGameObject.hpp"

cPlinkoBallGameObject::cPlinkoBallGameObject()
{
	this->type = "PlinkoBall";
	this->startingPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

cPlinkoBallGameObject::cPlinkoBallGameObject(std::string name, glm::vec3 startingPos)
{
	this->type = "PlinkoBall";
	this->name = name;
	this->startingPos = startingPos;
}

cPlinkoBallGameObject::cPlinkoBallGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj, mapMeshes);
	this->instatiateUniqueVariables(obj);
}

void cPlinkoBallGameObject::init()
{

}

void cPlinkoBallGameObject::update(float dt)
{
	if (this->position.y <= -1.0f)
	{
		this->position = this->startingPos;
		this->velocity = glm::vec3(0.0f);
	}
}

sMessage cPlinkoBallGameObject::message(sMessage const& msg)
{
	return sMessage();
}

void cPlinkoBallGameObject::instatiateUniqueVariables(Json::Value& obj)
{
	Json::Value uniques = obj["uniques"];
	for (unsigned j = 0; j < 3; ++j)
	{
		this->startingPos[j] = uniques["startingPos"][j].asFloat();
	}
}

void cPlinkoBallGameObject::serializeUniqueVariables(Json::Value& obj)
{
	Json::Value uniques = Json::objectValue;
	for (unsigned j = 0; j < 3; ++j)
	{
		uniques["startingPos"][j] = this->startingPos[j];
	}
	obj["uniques"] = uniques;
}
