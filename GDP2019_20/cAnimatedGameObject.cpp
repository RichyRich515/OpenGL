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
	this->action_timer -= dt;
	this->jump_timer -= dt;

	if (active)
	{
		bool running = false;
		bool walking = false;
		
		if (cKeyboardManager::keyDown('A'))
		{
			this->velocity = glm::vec3(1.0f, 0.0f, 0.0f);
			this->transform.setOrientation(glm::quatLookAt(-velocity, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		else if (cKeyboardManager::keyDown('D'))
		{
			this->velocity = glm::vec3(-1.0f, 0.0f, 0.0f);
			this->transform.setOrientation(glm::quatLookAt(-velocity, glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		else
		{
			this->velocity = glm::vec3(0.0f);
		}

		if (this->velocity.x != 0.0f)
		{
			if (cKeyboardManager::keyDown(GLFW_KEY_LEFT_SHIFT))
			{
				this->velocity *= glm::vec3(80.0f, 0.0f, 0.0f);
				running = true;
			}
			else
			{
				this->velocity *= glm::vec3(22.0f, 0.0f, 0.0f);
				walking = true;
			}
		}

		if (this->action_timer <= 0.0f)
		{
			if (running)
			{
				this->skinmesh.forceAnimation("Run", false);
			}
			else if (walking)
			{
				this->skinmesh.forceAnimation("Walk", false);
			}
			else
			{
				this->skinmesh.forceAnimation("Idle", true);
			}

			if (cKeyboardManager::keyPressed('E'))
			{
				this->skinmesh.forceAnimation("Punch", false, true);
				this->action_timer = this->skinmesh.skinmesh.FindAnimationTotalTime("Punch");
			}
			else if (cKeyboardManager::keyPressed(GLFW_KEY_SPACE))
			{
				this->skinmesh.forceAnimation("Jump", false, true);
				this->action_timer = this->skinmesh.skinmesh.FindAnimationTotalTime("Jump");
				this->jumping = true;
				this->jump_start_y = this->transform.position.y;
				this->jump_timer = this->action_timer * 0.7f;
			 	this->jump_speed = 40.0f / jump_timer;
			}
		}

		if (jumping)
		{
			this->velocity.y = jump_speed;
			if (this->jump_timer <= 0.0f)
			{
				this->transform.position.y = this->jump_start_y + 40.0f;
				this->velocity.y = 0.0f;
				this->jumping = false;
			}
		}

		this->transform.position += this->velocity * dt;


		if (!jumping)
		{
			// check if should fall
		}
		
		// check if should hit wall
		{

		}
	}

	this->graphics.update(dt, tt);
	this->skinmesh.update(dt, tt);
	this->mesh.update(dt, tt);
	this->transform.update(dt, tt);
}
