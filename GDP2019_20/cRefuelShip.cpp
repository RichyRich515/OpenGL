#include "cRefuelShip.hpp"

#include "cWorld.hpp"
#include <algorithm>

cRefuelShip::cRefuelShip()
{
	this->type = "refuelShip";
	this->_engine = nullptr;
	this->_weapon = nullptr;
	this->destination = nullptr;
}

cRefuelShip::cRefuelShip(std::string name)
{
	this->type = "refuelShip";
	this->_engine = nullptr;
	this->_weapon = nullptr;
	this->destination = nullptr;
}

cRefuelShip::cRefuelShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	instatiateBaseVariables(obj, mapMeshes);
	instatiateUniqueVariables(obj);
	this->type = "refuelShip";
}

cRefuelShip::~cRefuelShip()
{
	
}

void cRefuelShip::init()
{

}

void cRefuelShip::update(float dt)
{
	if (this->_engine && this->destination)
	{
		glm::vec3 dest = this->destination->position;
		float distToDest = glm::distance(this->position, dest);
		if (distToDest > 0.005f)
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
			sMessage msg = sMessage("refuel", 10.0f);
			// Don't care about return
			this->destination->message(msg);
			this->destination = nullptr;
		}
	}
	else 
	{
		// ask world for a stranded ship every update call when we dont already have one
		sMessage msg = sMessage("find stranded");
		void* vp = cWorld::getWorld()->message(msg).data.vp;
		if (vp != nullptr)
			this->destination = (cGameObject*)vp;
		else
			this->destination = nullptr;
	}
}

sMessage cRefuelShip::message(sMessage const& msg)
{
	return sMessage();
}

void cRefuelShip::instatiateUniqueVariables(Json::Value& obj)
{
	Json::Value uniques = obj["uniques"];
	this->_engine = nullptr;
	this->_weapon = nullptr;
	this->fuel_reservoir = uniques["fuel_reservoir"].asFloat();
	std::string destName = uniques["destination"].asString();

	cWorld* world = cWorld::getWorld();
	// Find destination
	auto itr = std::find_if(world->vecGameObjects.begin(), world->vecGameObjects.end(),
		[destName](const cGameObject* go) -> bool { return go->name == destName; });
	if (itr != world->vecGameObjects.end())
		destination = *itr;
	else
		destination = nullptr;
}

void cRefuelShip::serializeUniqueVariables(Json::Value& obj)
{
	Json::Value uniques = Json::objectValue;
	if (this->destination != nullptr)
		uniques["destination"] = this->destination->name;
	else
		uniques["destination"] = "";
	uniques["engine"] = this->_engine->serialize();
	uniques["weapon"] = this->_weapon->serialize();
	uniques["fuel_reservoir"] = this->fuel_reservoir;
	obj["uniques"] = uniques;
}
