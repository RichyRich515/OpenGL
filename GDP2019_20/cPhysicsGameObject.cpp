#include "cPhysicsGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <iostream>
#include "GLCommon.h"
#include "cPhysicsManager.hpp"
#include "JsonHelper.hpp"
#include "cClothMeshComponent.hpp"

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
	cPhysicsManager::getWorld()->RemoveComponent(this->physics);
	if (this->physics)
		delete this->physics;
	if (this->mesh)
		delete this->mesh;
}

void cPhysicsGameObject::instatiateBaseVariables(const Json::Value& obj)
{
	this->name = obj["name"] ? obj["name"].asString() : ""; 
	this->type = obj["type"] ? obj["type"].asString() : "";

	if (obj["graphics"])
		this->graphics.instatiateBaseVariables(obj["graphics"]);


	if (obj["physics"])
	{
		std::string type = obj["physics"]["type"] ? obj["physics"]["type"].asString() : "";
		if (type != "cloth" && obj["mesh"])
		{
			this->mesh = new cMeshComponent();
			this->mesh->instatiateBaseVariables(obj["mesh"]);
		}
		else
		{
			this->mesh = new cClothMeshComponent();
			this->mesh->instatiateBaseVariables(obj["mesh"]);
		}
		if (type == "ball")
		{
			nPhysics::sBallDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Radius = obj["physics"]["radius"] ? obj["physics"]["radius"].asFloat() : 1.0f;
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateBall(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "plane")
		{
			glm::vec3 point = obj["physics"]["point"] ? Json::toVec3(obj["physics"]["point"]) : glm::vec3(0.0f);

			nPhysics::sPlaneDef def;
			def.Normal = obj["physics"]["normal"] ? Json::toVec3(obj["physics"]["normal"]) : glm::vec3(0.0f);
			def.Constant = glm::dot(def.Normal, point);

			this->physics = cPhysicsManager::getFactory()->CreatePlane(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
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

glm::vec3 cPhysicsGameObject::getPosition()
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

void cPhysicsGameObject::preFrame(float dt, float tt)
{
}

void cPhysicsGameObject::render(float dt, float tt)
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

void cPhysicsGameObject::update(float dt, float tt)
{
	this->graphics.update(dt, tt);
	this->mesh->update(dt, tt);
}

sMessage cPhysicsGameObject::message(sMessage const& msg)
{
	return sMessage();
}
