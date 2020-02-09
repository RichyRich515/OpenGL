#include "cPhysicsGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <iostream>
#include "GLCommon.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

cPhysicsGameObject::cPhysicsGameObject()
{
	this->name = "";
	this->type = "basic";
}

cPhysicsGameObject::cPhysicsGameObject(std::string name)
{
	this->name = name;
	this->type = "basic";
}

cPhysicsGameObject::cPhysicsGameObject(Json::Value& obj)
{
	this->instatiateBaseVariables(obj);
}

cPhysicsGameObject::cPhysicsGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj);
}

cPhysicsGameObject::~cPhysicsGameObject()
{

}

void cPhysicsGameObject::instatiateBaseVariables(const Json::Value& obj)
{
	this->name = obj["name"] ? obj["name"].asString() : ""; 
	this->type = obj["type"] ? obj["type"].asString() : "";

	if (obj["graphics"])
		this->graphics.instatiateBaseVariables(obj["graphics"]);

	if (obj["mesh"])
		this->mesh.instatiateBaseVariables(obj["mesh"]);

	if (obj["physics"])
	{
		// TODO: physics

		//this->physics = cPhysicsManager::getCurrentFactory()->create
	}
}

void cPhysicsGameObject::instatiateUniqueVariables(const Json::Value& obj)
{
	// None for base game object
}

void cPhysicsGameObject::serializeJSONObject(Json::Value& obj)
{
	obj = Json::objectValue;
	obj["name"] = this->name;
	obj["type"] = this->type;


	serializeUniqueVariables(obj);
}

void cPhysicsGameObject::serializeUniqueVariables(Json::Value& obj)
{
	// None for base game object
}


void cPhysicsGameObject::init()
{
}

eComponentType cPhysicsGameObject::getType()
{
	return eComponentType::GameObject;
}

void cPhysicsGameObject::preFrame()
{
}

void cPhysicsGameObject::render()
{
	glm::mat4 m(1.0f);
	this->physics->GetTransform(m);
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

void cPhysicsGameObject::update(float dt, float tt)
{
	this->graphics.update(dt, tt);
	this->mesh.update(dt, tt);
}

sMessage cPhysicsGameObject::message(sMessage const& msg)
{
	return sMessage();
}
