#include "cAnimatedGameObject.hpp"
#include "cShaderManager.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "cKeyboardManager.hpp"
#include "cWorld.hpp"
#include <iostream>

glm::vec3 cAnimatedGameObject::getPosition()
{
	return this->transform.getPosition();
}

sMessage cAnimatedGameObject::message(sMessage const& msg)
{
	return sMessage();
}

void cAnimatedGameObject::init()
{
}

cAnimatedGameObject::cAnimatedGameObject()
{
}

cAnimatedGameObject::~cAnimatedGameObject()
{
	if (this->physics)
		delete this->physics;
}

eComponentType cAnimatedGameObject::getType()
{
	return eComponentType::GameObject;
}

void cAnimatedGameObject::preFrame(float dt, float tt)
{
}

void cAnimatedGameObject::render(float dt, float tt)
{
	if (!this->graphics.visible)
		return;

	auto pShader = cShaderManager::getCurrentShader();
	glUniform1f(pShader->getUniformLocID("isSkinnedMesh"), (float)GL_TRUE);

	glm::mat4 m = this->transform.matWorld;
	m *= glm::scale(glm::mat4(1.0f), glm::vec3(mesh.scale));
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(this->transform.inverseTransposeMatWorld));
	glUniform4f(pShader->getUniformLocID("params1"), dt, tt, 0.0f, 0.0f);
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render(dt, tt);
	this->skinmesh.render(dt, tt);
	this->mesh.render(dt, tt);

	glUniform1f(pShader->getUniformLocID("isSkinnedMesh"), (float)GL_FALSE);
}

void cAnimatedGameObject::update(float dt, float tt)
{
	this->action_timer -= dt;
	this->graphics.update(dt, tt);
	this->skinmesh.update(dt, tt);
	this->mesh.update(dt, tt);

	if (!this->physics->CanJump())
	{
		if (!falling && action_timer <= 0.0f)
		{
			action_timer = 0.25f;
			falling = true;
		}
		else if (action_timer <= 0.0f)
		{
			this->skinmesh.forceAnimation("Fall", true);
		}
	}
	else
	{
		action_timer = 0.0f;
		falling = false;
	}

	if (!falling)
	{
		if (cKeyboardManager::keyDown('W'))
		{
			this->skinmesh.forceAnimation("Walk", true);
		}
		else if (cKeyboardManager::keyDown('A'))
		{
			this->skinmesh.forceAnimation("Strafe Left", true);
		}
		else if (cKeyboardManager::keyDown('D'))
		{
			this->skinmesh.forceAnimation("Strafe Right", true);
		}
		else if (cKeyboardManager::keyDown('S'))
		{
			this->skinmesh.forceAnimation("Walk Back", true);
		}
		else
		{
			this->skinmesh.forceAnimation("Idle", true);
		}
	}
}
