#include "cAnimatedGameObject.hpp"
#include "cShaderManager.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "cKeyboardManager.hpp"
#include "cCamera.hpp"
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
	//this->skinmesh.default_animation = "Idle";
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
	m *= glm::scale(glm::mat4(1.0), glm::vec3(mesh.scale));
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

	//if (active)
	//{
	//	bool running = false;
	//	bool walking = false;
	//	if (punching && action_timer > 0.5f && action_timer < 0.75f)
	//	{
	//		for (auto robot : robots)
	//		{
	//			if (glm::distance(robot->getPosition(), this->transform.position + glm::vec3(dir * 10.0f, 0.0f, 0.0f)) < 5.5f)
	//			{
	//				robot->message(sMessage("hit"));
	//			}
	//		}
	//	}


	//	if (cKeyboardManager::keyDown('A') || cKeyboardManager::keyDown(GLFW_KEY_LEFT))
	//	{
	//		this->velocity = glm::vec3(1.0f, 0.0f, 0.0f);
	//		this->transform.setOrientation(glm::quatLookAt(-velocity, glm::vec3(0.0f, 1.0f, 0.0f)));
	//		this->dir = 1;
	//	}
	//	else if (cKeyboardManager::keyDown('D') || cKeyboardManager::keyDown(GLFW_KEY_RIGHT))
	//	{
	//		this->velocity = glm::vec3(-1.0f, 0.0f, 0.0f);
	//		this->transform.setOrientation(glm::quatLookAt(-velocity, glm::vec3(0.0f, 1.0f, 0.0f)));
	//		this->dir = -1;
	//	}
	//	else
	//	{
	//		this->velocity = glm::vec3(0.0f);
	//	}

	//	if (this->velocity.x != 0.0f)
	//	{
	//		if (cKeyboardManager::keyDown(GLFW_KEY_LEFT_SHIFT) || cKeyboardManager::keyDown(GLFW_KEY_RIGHT_SHIFT))
	//		{
	//			this->velocity *= glm::vec3(80.0f, 0.0f, 0.0f);
	//			running = true;
	//		}
	//		else
	//		{
	//			this->velocity *= glm::vec3(22.0f, 0.0f, 0.0f);
	//			walking = true;
	//		}
	//	}

	//	if (this->action_timer <= 0.0f && !this->falling)
	//	{
	//		this->punching = false;
	//		if (running)
	//		{
	//			this->skinmesh.forceAnimation("Run", false);
	//		}
	//		else if (walking)
	//		{
	//			this->skinmesh.forceAnimation("Walk", false);
	//		}
	//		else
	//		{
	//			this->skinmesh.forceAnimation("Idle", true);
	//		}

	//		if (cKeyboardManager::keyPressed('E') || cKeyboardManager::keyPressed(GLFW_KEY_ENTER))
	//		{
	//			this->skinmesh.forceAnimation("Punch", false, true);
	//			this->action_timer = this->skinmesh.skinmesh.FindAnimationTotalTime("Punch");
	//			this->punching = true;
	//		}
	//		else if (cKeyboardManager::keyPressed(GLFW_KEY_SPACE) || cKeyboardManager::keyPressed('W') || cKeyboardManager::keyPressed(GLFW_KEY_UP))
	//		{
	//			this->skinmesh.forceAnimation("Jump", false, true);
	//			this->action_timer = this->skinmesh.skinmesh.FindAnimationTotalTime("Jump");
	//			this->jumping = true;
	//			this->jump_start_y = this->transform.position.y;
	//			this->jump_timer = this->action_timer * 0.7f;
	//			this->jump_speed = 40.0f / jump_timer;
	//		}
	//		else if (this->velocity.x != 0.0f && cKeyboardManager::keyPressed('S') || cKeyboardManager::keyPressed(GLFW_KEY_DOWN))
	//		{
	//			this->skinmesh.forceAnimation("Roll", false, true);
	//			this->action_timer = this->skinmesh.skinmesh.FindAnimationTotalTime("Roll");
	//		}
	//	}
	//	else if (this->action_timer <= 0.0f && this->falling)
	//	{
	//		this->skinmesh.forceAnimation("Fall", true);
	//	}

	//	if (jumping)
	//	{
	//		this->velocity.y = jump_speed;
	//		if (this->jump_timer <= 0.0f)
	//		{
	//			this->transform.position.y = this->jump_start_y + 40.0f;
	//			this->velocity.y = 0.0f;
	//			this->jumping = false;
	//		}
	//	}

	//	//get current tile
	//	int x = fmodf((this->transform.position.x + (*level).size() * 5.0f) / 10.0f, 27.0f);
	//	int y = -fmodf((this->transform.position.y) / 10.0f, 16.0f) + (*level)[0].size() - 1;

	//	float xin = fmodf((this->transform.position.x + (*level).size() * 5.0f), 10.0f);
	//	float yin = fmodf((this->transform.position.y), 10.0f);

	//	// off top of screen
	//	if (!jumping && this->transform.position.y > 120.0f)
	//	{
	//		this->falling = true;
	//		this->velocity.y = -80.0f;
	//	}

	//	if (x > 0 && x < (*level).size() && y > 0 && y < (*level)[0].size())
	//	{
	//		// check stop falling
	//		if (this->falling && this->transform.position.y <= 121.0f)
	//		{
	//			if ((*level)[x][y] == 1 && yin <= 9.0f)
	//			{
	//				this->falling = false;
	//				this->transform.position.y = ((*level)[0].size() - y - 1) * 10.0f;
	//			}
	//		}

	//		// check if should fall
	//		if (!jumping && !falling)
	//		{
	//			if ((*level)[x][y] == 0)
	//			{
	//				this->falling = true;
	//			}
	//		}

	//		if (this->falling)
	//		{
	//			if (this->transform.position.y < 0.0f)
	//				this->transform.position.y = 0.0f;
	//			else
	//				this->velocity.y = -80.0f;
	//		}


	//		// check if should hit wall
	//		if (!jumping && !falling)
	//		{
	//			if (this->velocity.x < 0.0f)
	//			{
	//				if ((*level)[x - 1][y - 1]
	//					|| (*level)[x - 1][y - 2]
	//					|| (*level)[x - 1][y - 3])
	//				{
	//					this->velocity.x = 0.0f;
	//				}
	//			}
	//			else if (this->velocity.x > 0.0f)
	//			{
	//				if ((*level)[x + 1][y - 1]
	//					|| (*level)[x + 1][y - 2]
	//					|| (*level)[x + 1][y - 3])
	//				{
	//					this->velocity.x = 0.0f;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			// falling or jumping 
	//			if ((this->velocity.x < 0.0f && this->transform.position.x < -115.0f)
	//				|| (this->velocity.x > 0.0f && this->transform.position.x > 115.0f))
	//			{
	//				this->velocity.x = 0.0f;
	//			}
	//		}
	//	}

	//	this->transform.position += this->velocity * dt;

	//	this->graphics.update(dt, tt);
	//	this->skinmesh.update(dt, tt);
	//	this->mesh.update(dt, tt);
	//	this->transform.update(dt, tt);
	//}
	//else
	//{
	//	if (!this->alive)
	//	{
	//		if (this->action_timer <= 0.0f)
	//		{
	//			this->alive = true;
	//			return;
	//		}
	//		this->skinmesh.update(dt, tt);
	//	}

	//}

}
