#include "cAnimatedGameObject.hpp"
#include "cShaderManager.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "cKeyboardManager.hpp"
#include "cCamera.hpp"

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
	//this->skinmesh.default_animation = "Idle";
}

cAnimatedGameObject::cAnimatedGameObject()
{
}

eComponentType cAnimatedGameObject::getType()
{
	return eComponentType::GameObject;
}

void cAnimatedGameObject::preFrame()
{
}

void cAnimatedGameObject::render()
{
	if (!this->graphics.visible)
		return;

	auto pShader = cShaderManager::getCurrentShader();
	glUniform1f(pShader->getUniformLocID("isSkinnedMesh"), (float)GL_TRUE);

	this->transform.updateMatricis();
	glm::mat4 m = this->transform.matWorld;
	m *= glm::scale(glm::mat4(1.0), glm::vec3(mesh.scale));
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render();
	this->skinmesh.render();
	this->mesh.render();

	glUniform1f(pShader->getUniformLocID("isSkinnedMesh"), (float)GL_FALSE);
}

void cAnimatedGameObject::update(float dt, float tt)
{
	if (cKeyboardManager::keyDown('W'))
	{
		if (cKeyboardManager::keyDown(GLFW_KEY_LEFT_SHIFT)) 
		{
			this->velocity = glm::vec3(0.0f, 0.0f, 80.0f);
			this->skinmesh.forceAnimation("Run", 0);
		}
		else
		{
			this->velocity = glm::vec3(0.0f, 0.0f, 22.0f);
			this->skinmesh.forceAnimation("Walk", 0);
		}
	}
	else if (cKeyboardManager::keyDown('A'))
	{
		this->velocity = glm::vec3(40.0f, 0.0f, 0.0f);
		this->skinmesh.forceAnimation("Strafe Left", 0);
	}
	else if (cKeyboardManager::keyDown('D'))
	{
		this->velocity.x = -40.0f;
		this->skinmesh.forceAnimation("Strafe Right", 0);
	}
	else
	{
		this->skinmesh.queueAnimation("Idle", 1);
		this->velocity = glm::vec3(0.0f);
	}

	if (cKeyboardManager::keyPressed('E'))
	{
		this->skinmesh.forceAnimation("Punch", 1);
	}
	else if (cKeyboardManager::keyPressed(GLFW_KEY_SPACE))
	{
		this->skinmesh.forceAnimation("Jump", 1);
	}
	

	if (glm::length(this->velocity) > 0.0f)
	{
		this->transform.setOrientation(
			glm::quatLookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::quat(glm::vec3(0.0f, glm::radians(180.0f), 0.0f)));

	}
		
	this->transform.position += this->velocity * dt;

	this->graphics.update(dt, tt);
	this->skinmesh.update(dt, tt);
	this->mesh.update(dt, tt);
	this->transform.update(dt, tt);
}
