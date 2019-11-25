#include "cWorld.hpp"

#include <algorithm>

cWorld cWorld::_world;

bool cWorld::debugMode;
cDebugRenderer* cWorld::pDebugRenderer;

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

void cWorld::deferredAddGameObject(cGameObject* go)
{
	deferred_adds.emplace(go);
}

void cWorld::deferredDeleteGameObject(cGameObject* go)
{
	deferred_deletes.emplace(go);
}

void cWorld::doDeferredActions()
{
	// delete game objects
	if (deferred_deletes.size() > 0)
	{
		for (std::set<cGameObject*>::iterator itr = deferred_deletes.begin(); itr != deferred_deletes.end(); ++itr)
		{
			std::vector<cGameObject*>::iterator to_delete_itr = std::find(vecGameObjects.begin(), vecGameObjects.end(), (*itr));
			if (to_delete_itr != vecGameObjects.end())
			{
				delete (*to_delete_itr);
				vecGameObjects.erase(to_delete_itr);
			}
		}
		deferred_deletes.clear();
	}

	// add game objects
	if (deferred_adds.size() > 0)
	{
		for (std::set<cGameObject*>::iterator itr = deferred_adds.begin(); itr != deferred_adds.end(); ++itr)
		{
			vecGameObjects.push_back(*itr);
		}
		deferred_adds.clear();
	}
}

cWorld::cWorld()
{
	// Empty constructor
}
