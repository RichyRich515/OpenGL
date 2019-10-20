#include "cWorld.hpp"

cWorld cWorld::_world;

sMessage cWorld::message(sMessage const& msg)
{
	return sMessage();
}

void cWorld::addGameObject(cGameObject* go)
{
	vecGameObjects.push_back(go);
}

void cWorld::addLight(cLight* light)
{
	vecLights.push_back(light);
}

cWorld* cWorld::getWorld()
{
	return &(cWorld::_world);
}

cWorld::cWorld()
{
	// Empty constructor
}
