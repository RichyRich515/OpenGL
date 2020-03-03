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
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render();
	this->mesh.render();
}

void cGameObject::update(float dt, float tt)
{
	for (auto robot : robots)
		if (robot != this && robot->message(sMessage("dying?")).name == "yes")
			return;

	if (this->active)
	{
		this->immune_timer -= dt;

		if (this->alive)
		{
			if (this->immune_timer <= 0.0f)
			{
				this->graphics.color = glm::vec4(0.25f, 0.75f, 0.25f, 1.0f);
				this->transform.position.x += this->dir * 10.0f * dt;
				if (this->transform.position.x <= this->right_most_pos)
					this->dir = 1;
				else if (this->transform.position.x >= this->left_most_pos)
					this->dir = -1;

				if (this->dir)
					this->transform.orientation = glm::quatLookAt(glm::vec3(-dir, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				this->transform.updateMatricis();

				if (glm::distance(this->transform.position, this->player->getPosition()) < 5.0f)
				{
					this->player->message(sMessage("die"));
				}
			}
		}
		else
		{
			// dead
			if (this->dead_state == 1 && this->mesh.scale < 4.8)
			{
				this->mesh.scale += dt;
			}
			else
			{
				this->dead_state = 2;
			}

			if (this->dead_state == 2 && this->mesh.scale > 0.0f)
			{
				this->mesh.scale -= dt * 2.0f;
			}
			else if (this->dead_state == 2)
			{
				this->dead_state = 3;
				this->active = false;
			}
		}

		this->graphics.update(dt, tt);
		this->mesh.update(dt, tt);
		this->transform.update(dt, tt);

	}
}

sMessage cGameObject::message(sMessage const& msg)
{
	if (this->active)
	{
		if (msg.name == "dying?")
		{
			if (!this->alive && (this->dead_state == 1 || dead_state == 2))
			{
				return sMessage("yes");
			}
		}
		else if (msg.name == "stop")
		{
			this->dir = 0;
		}
		else if (msg.name == "hit")
		{
			if (this->immune_timer <= 0.0f)
			{
				--this->health;
				this->immune_timer = 1.0f;
				this->graphics.color = glm::vec4(0.75f, 0.25f, 0.25f, 1.0f);
				if (health <= 0)
				{
					this->alive = false;
					return sMessage("killed");
				}
			}
		}
	}
	
	return sMessage();
}
