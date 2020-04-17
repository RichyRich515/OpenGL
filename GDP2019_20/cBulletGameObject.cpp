#include "cBulletGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <iostream>
#include "GLCommon.h"
#include "cPhysicsManager.hpp"
#include "JsonHelper.hpp"
#include "cWorld.hpp"
#include "cClothMeshComponent.hpp"

cBulletGameObject::cBulletGameObject()
{
	this->name = "";
	this->type = "bullet";
}

cBulletGameObject::cBulletGameObject(std::string name)
{
	this->name = name;
	this->type = "bullet";
}

cBulletGameObject::cBulletGameObject(Json::Value& obj)
{
	this->instatiateBaseVariables(obj);
}

cBulletGameObject::cBulletGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj);
}

cBulletGameObject::~cBulletGameObject()
{
	cPhysicsManager::getWorld()->RemoveComponent(this->physics);
	if (this->physics)
		delete this->physics;
	if (this->mesh)
		delete this->mesh;
}

void cBulletGameObject::instatiateBaseVariables(const Json::Value& obj)
{
}

void cBulletGameObject::instatiateUniqueVariables(const Json::Value& obj)
{
}

void cBulletGameObject::serializeJSONObject(Json::Value& obj)
{
}

void cBulletGameObject::serializeUniqueVariables(Json::Value& obj)
{
	// None for base game object
}


void cBulletGameObject::init()
{
}

eComponentType cBulletGameObject::getType()
{
	return eComponentType::GameObject;
}

glm::vec3 cBulletGameObject::getPosition()
{
	if (this->physics)
	{
		glm::mat4 transform;
		this->physics->GetTransform(transform);
		return transform[3];
	}
	else
	{
		return glm::vec3(0.0f);
	}
}

void cBulletGameObject::preFrame(float dt, float tt)
{
}

void cBulletGameObject::render(float dt, float tt)
{
	if (!this->graphics.visible)
		return;

	glm::mat4 m(1.0f);
	this->physics->GetTransform(m);
	m *= glm::scale(glm::mat4(1.0), glm::vec3(mesh->scale));
	cShaderManager::cShaderProgram* pShader = cShaderManager::getCurrentShader();
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render(dt, tt);
	this->mesh->render(dt, tt);
}

void cBulletGameObject::update(float dt, float tt)
{
	if (life_time <= 0)
	{
		cWorld::getWorld()->deferredDeleteGameObject(this);
	}
	life_time -= dt;


	this->graphics.update(dt, tt);
	this->mesh->update(dt, tt);
}

sMessage cBulletGameObject::message(sMessage const& msg)
{
	return sMessage();
}
