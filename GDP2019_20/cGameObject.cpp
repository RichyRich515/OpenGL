#include "cGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <iostream>
#include "GLCommon.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

cGameObject::cGameObject()
{
	this->name = "";
	this->type = "basic";
}

cGameObject::cGameObject(std::string name)
{
	this->name = name;
	this->type = "basic";
}

cGameObject::cGameObject(Json::Value& obj)
{
	this->instatiateBaseVariables(obj);
}

cGameObject::cGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj);
}

cGameObject::~cGameObject()
{

}

void cGameObject::instatiateBaseVariables(const Json::Value& obj)
{
	this->name = obj["name"] ? obj["name"].asString() : "";
	this->type = obj["type"] ? obj["type"].asString() : "";

	if (obj["graphics"])
		this->graphics.instatiateBaseVariables(obj["graphics"]);

	if (obj["mesh"])
		this->mesh.instatiateBaseVariables(obj["mesh"]);

	if (obj["transform"])
		this->transform.instatiateBaseVariables(obj["transform"]);
}

void cGameObject::instatiateUniqueVariables(const Json::Value& obj)
{
	// None for base game object
}

void cGameObject::serializeJSONObject(Json::Value& obj)
{
	obj = Json::objectValue;
	obj["name"] = this->name;
	obj["type"] = this->type;


	serializeUniqueVariables(obj);
}

void cGameObject::serializeUniqueVariables(Json::Value& obj)
{
	// None for base game object
}


void cGameObject::init()
{
}

eComponentType cGameObject::getType()
{
	return eComponentType::GameObject;
}

glm::vec3 cGameObject::getPosition()
{
	return this->transform.position;
}

void cGameObject::preFrame()
{
}

void cGameObject::render()
{
	if (!this->graphics.visible)
		return;

	glm::mat4 m = this->transform.matWorld;
	m *= glm::scale(glm::mat4(1.0), glm::vec3(mesh.scale));
	cShaderManager::cShaderProgram* pShader = cShaderManager::getCurrentShader();
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
	glUniform4f(pShader->getUniformLocID("params2"),
		0.0f,
		this->name == "terrain" ? 1.0f : 0.0f,
		this->name == "ocean" || this->name == "sand_floor" ? 1.0f : 0.0f,
		0.0f);

	this->graphics.render();
	this->mesh.render();
}

void cGameObject::update(float dt, float tt)
{
	this->graphics.update(dt, tt);
	this->mesh.update(dt, tt);
	this->transform.update(dt, tt);
}

sMessage cGameObject::message(sMessage const& msg)
{
	return sMessage();
}
