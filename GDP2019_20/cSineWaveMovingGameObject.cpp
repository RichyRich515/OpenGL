#include "cSineWaveMovingGameObject.hpp"

cSineWaveMovingGameObject::cSineWaveMovingGameObject()
{
	this->name = "";
	this->meshName = "";
	this->type = "SineWaveMoving";
	this->mesh = NULL;
	this->position = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->scale = 1.0f;
	this->color = glm::vec4(1.0f);
	this->specular = glm::vec4(1.0f);
	this->wireFrame = false;
	this->visible = true;
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->inverseMass = 0.0f;
	this->moving = true;
	this->counter = 0;
	this->startingPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

cSineWaveMovingGameObject::cSineWaveMovingGameObject(std::string name)
{
	this->name = name;
	this->meshName = "";
	this->type = "SineWaveMoving";
	this->mesh = NULL;
	this->position = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->scale = 1.0f;
	this->color = glm::vec4(1.0f);
	this->specular = glm::vec4(1.0f);
	this->wireFrame = false;
	this->visible = true;
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->inverseMass = 0.0f;
	this->moving = true;
	this->counter = 0;
	this->startingPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

cSineWaveMovingGameObject::cSineWaveMovingGameObject(Json::Value obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj, mapMeshes);
	this->moving = true;
	this->counter = 0;
	this->startingPos = glm::vec3(0.0f, 0.0f, 0.0f);
	instatiateUniqueVariables(obj);
}

void cSineWaveMovingGameObject::init()
{
	// nothing
}

void cSineWaveMovingGameObject::update(float dt)
{
	if (this->moving)
	{
		this->counter += dt;

		this->position.x = this->startingPos.x + sin(this->counter);
	}
}

sMessage cSineWaveMovingGameObject::message(sMessage const& msg)
{
	if (msg.name == "set moving")
	{
		this->moving = msg.data.b;
	}
	return sMessage();
}

void cSineWaveMovingGameObject::instatiateUniqueVariables(Json::Value obj)
{

}
