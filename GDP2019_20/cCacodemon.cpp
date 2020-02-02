#include "cCacodemon.hpp"
#include "cWorld.hpp"
#include "SteeringFunctions.hpp"
#include "cBullet.hpp"
#include "rwmath.hpp"

cCacodemon::cCacodemon(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj, mapMeshes);
	this->instatiateUniqueVariables(obj);
}

void cCacodemon::init()
{
	this->setPosition(glm::vec3(rw::rand_in_range(-127.0f, 127.0f), 2.0f, rw::rand_in_range(-127.0f, 127.0f)));
	this->velocity = glm::vec3(0.0f);
	this->face_texture = this->textures[0];
	this->face_discard_texture = this->discardmap;
	this->back_texture = this->textures[1];
	this->back_discard_texture = this->textures[2];
	this->back_texture.blend = 1.0f;
	this->back_discard_texture.blend = 0.5f;

	this->enemyType = (eEnemyType)(rand() % 4);
}

void cCacodemon::update(float dt, float tt)
{
	glm::vec3 pos = this->getPosition();

	bool player_is_facing = glm::dot(cWorld::pCamera->forward, glm::normalize(cWorld::pCamera->position - pos)) > 0.0f;

	switch (this->enemyType)
	{
	case eEnemyType::type_a:
		if (player_is_facing)
		{
			this->velocity += cSteeringFunctions::steerSeekArrive(pos, cWorld::pCamera->position, this->velocity, 0.0f, this->maxSpeed, dt);
			this->color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
		}
		else
		{
			this->velocity += cSteeringFunctions::steerFlee(pos, cWorld::pCamera->position, this->velocity, this->maxSpeed, dt);
			this->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		break;
	case eEnemyType::type_b:
	{
		this->velocity += cSteeringFunctions::steerPursue(pos, cWorld::pCamera->position, cWorld::pCamera->velocity, this->velocity, this->maxSpeed, dt);
		std::vector<cGameObject*> bullets;
		cWorld::getWorld()->message(sMessage("GetObjects:bullet", (void*)&bullets));
		if (bullets.size() > 0)
		{
			this->velocity += cSteeringFunctions::steerAvoid(pos, this->velocity, bullets, 2.5f, 5.0f, this->maxSpeed, dt);
			this->color = glm::vec4(0.25f, 1.0f, 0.25f, 1.0f);
		}
		else
		{
			this->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}
		break;
	case eEnemyType::type_c:
		shot_timer -= dt;
		if (glm::distance(pos, cWorld::pCamera->position) <= 25.0f)
		{
			if (shot_timer <= 0.0f)
			{
				cBullet* bullet = new cBullet();
				bullet->from_enemy = true;
				bullet->name = "enemy_bullet";
				bullet->meshName = "Cacodemon";
				bullet->textures[0].fileName = "fireball.bmp";
				bullet->textures[0].blend = 1.0f;
				bullet->textures[0].tiling = 1.0f;
				bullet->scale = 0.25f;
				bullet->setPosition(pos);
				bullet->velocity = glm::normalize(cWorld::pCamera->position - pos) * 15.0f;
				bullet->lighting = false;
				bullet->setOrientation(glm::quatLookAt(glm::normalize(bullet->velocity), glm::vec3(0.0f, 1.0f, 0.0f)));
				bullet->color = glm::vec4(1.0f, 0.3f, 0.3f, 0.5f);
				cWorld::getWorld()->deferredAddGameObject(bullet);
				shot_timer = 6.0f;
			}
			this->velocity *= 0.0f;
			this->color = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
		}
		else
		{
			this->velocity += cSteeringFunctions::steerSeekArrive(pos, cWorld::pCamera->position, this->velocity, 15.0f, this->maxSpeed, dt);
			this->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		break;
	case eEnemyType::type_d:
		this->wander_idle_timer -= dt;
		if (this->wander_idle_timer <= 0.0f)
		{
			if (this->idling)
				this->wander_idle_timer = 6.0f;
			else
				this->wander_idle_timer = 3.0f;
			this->idling = !this->idling;
		}
		if (this->idling)
		{
			this->velocity *= 0.0f;
			this->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			this->velocity += cSteeringFunctions::steerWander(pos, this->velocity, 5.0f, 2.5f, this->maxSpeed, dt);
			this->color = glm::vec4(0.7f, 0.7f, 0.0f, 1.0f);
		}
		break;
	}

	if (pos.x > 127.0f)
	{
		this->velocity.x = 0.0f;
		pos.x = 127.0f;
		this->setPosition(pos);
	}
	else if (pos.x < -127.0f)
	{
		this->velocity.x = 0.0f;
		pos.x = -127.0f;
		this->setPosition(pos);
	}

	if (pos.z > 127.0f)
	{
		this->velocity.z = 0.0f;
		pos.z = 127.0f;
		this->setPosition(pos);
	}
	else if (pos.z < -127.0f)
	{
		this->velocity.z = 0.0f;
		pos.z = -127.0f;
		this->setPosition(pos);
	}

	if (glm::distance(pos, cWorld::pCamera->position) <= 1.0f)
	{
		cWorld::getWorld()->message(sMessage("Reset"));
	}
	
	float speed = glm::length(this->velocity);

	bool looking_towards_player = false;
	if (speed > 0.0f)
	{
		looking_towards_player = glm::dot(glm::normalize(this->velocity), glm::normalize(cWorld::pCamera->position - pos)) > 0.0f;
	}
	else
	{
		looking_towards_player = glm::dot(glm::normalize(cWorld::pCamera->position - pos), glm::normalize(cWorld::pCamera->position - pos)) > 0.0f;
	}

	if (currentTexture != 0 && looking_towards_player)
	{
		// use face texture
		this->textures[0] = this->face_texture;
		this->discardmap = this->face_discard_texture;
		currentTexture = 0;
	}
	else if (currentTexture != 1 && !looking_towards_player)
	{
		// use back texture
		this->textures[0] = this->back_texture;
		this->discardmap = this->back_discard_texture;
		currentTexture = 1;
	}
	 
	
	if (speed > 0.0f)
	{
		if (speed > this->maxSpeed)
		{
			this->velocity = glm::normalize(this->velocity) * this->maxSpeed;
			speed = this->maxSpeed;
		}
		this->setOrientation(glm::quatLookAt(glm::normalize(this->velocity), glm::vec3(0.0f, 1.0f, 0.0f)));
	}
	else
	{
		this->setOrientation(glm::quatLookAt(glm::normalize(cWorld::pCamera->position - pos), glm::vec3(0.0f, 1.0f, 0.0f)));
	}
}
