#include "cBasicShip.hpp"

#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "cWorld.hpp"

cBasicShip::cBasicShip()
{
	this->type = "basicShip";
	this->_engine = nullptr;
	this->_weapon = nullptr;
	this->destination = nullptr;
}

cBasicShip::cBasicShip(std::string name)
{
	this->type = "basicShip";
	this->_engine = nullptr;
	this->_weapon = nullptr;
	this->destination = nullptr;
}

cBasicShip::cBasicShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	instatiateBaseVariables(obj, mapMeshes);
	instatiateUniqueVariables(obj);
	this->type = "basicShip";
}

cBasicShip::~cBasicShip()
{
	if (this->_engine)
		delete this->_engine;
	if (this->_weapon)
		delete this->_weapon;
}

void cBasicShip::init()
{
	// Nothing
}

void cBasicShip::update(float dt)
{
	if (this->_engine && this->destination)
	{
		glm::vec3 dest = this->destination->position;
		float distToDest = glm::distance(this->position, dest);
		if (distToDest > 0.5f)
		{
			glm::vec3 direction = normalize(dest - this->position);
			if (this->_engine->getFuel() > 0.0f)
			{
				// Move towards destination
				glm::vec3 vel = direction * this->_engine->getSpeed() * dt;
				glm::vec3 newPos = this->position + vel;
				if (length(vel) >= distToDest)
				{
					newPos = dest;
				}
				else if (glm::distance(newPos, dest) >= distToDest)
				{
					newPos = dest;
				}
				this->position = newPos;
				rotation.y = glm::mix(rotation.y, atan2(direction.x, direction.z), dt);
				this->_engine->consumeFuel(dt);
			}
		}
		else // at destination
		{
			sMessage msg = sMessage("requestfuel", 10.0f);
			// send return message to self
			msg = destination->message(msg);
			this->message(msg);

			// HACK: planets/asteroids are one of the first 5 objects
			cWorld* world = cWorld::getWorld();
			destination = world->vecGameObjects[rand() % 5];
		}
	}

	//if (this->_weapon)
	//{
	//	// TODO: start blastin
	//}
}

sMessage cBasicShip::message(sMessage const& msg)
{
	if (msg.name == "refuel")
		this->_engine->increaseFuel(msg.data.f);
	else if (msg.name == "Stranded?")
	{
		if (_engine->getFuel() <= 0.0f)
			return sMessage("Yes");
	}
	return sMessage();
}

void cBasicShip::instatiateUniqueVariables(Json::Value& obj)
{
	Json::Value uniques = obj["uniques"];
	this->_engine = nullptr;
	this->_weapon = nullptr;
	std::string destName = uniques["destination"].asString();

	// HACK: planets/asteroids are one of the first 5 objects
	cWorld* world = cWorld::getWorld();
	// Find planet
	auto itr = std::find_if(world->vecGameObjects.begin(), world->vecGameObjects.end(), 
								[destName](const cGameObject* go) -> bool { return go->name == destName;});
	if (itr != world->vecGameObjects.end())
		destination = *itr;
	else
		destination = nullptr;
}

void cBasicShip::serializeUniqueVariables(Json::Value& obj)
{
	Json::Value uniques = Json::objectValue;
	if (this->destination != nullptr)
		uniques["destination"] = this->destination->name;
	else
		uniques["destination"] = "";
	uniques["engine"] = this->_engine->serialize();
	uniques["weapon"] = this->_weapon->serialize();
	
	obj["uniques"] = uniques;
}

void cBasicShip::setEngine(iEngine* engine)
{
	if (this->_engine)
		delete this->_engine;
	this->_engine = engine;
}

void cBasicShip::setWeapon(iWeapon* weapon)
{
	if (this->_weapon)
		delete this->_weapon;
	this->_weapon = weapon;
}
