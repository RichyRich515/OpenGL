#pragma once

#include <vector>
#include <set>

#include "iMessageable.hpp"
#include "iGameObjectFactory.hpp"
#include "cGameObject.hpp"
#include "DebugRenderer/cDebugRenderer.h"
#include "cLight.hpp"

// Singleton world object
class cWorld : iMessageable
{
public:
	virtual sMessage message(sMessage const& msg);

	void addGameObject(iGameObject* go);
	void addLight(cLight* light);

	static cWorld* getWorld();

	std::vector<iGameObject*> vecGameObjects;
	std::vector<cLight*> vecLights;

	iGameObjectFactory* gameObjectFactory;

	void deferredAddGameObject(iGameObject* go);
	void deferredDeleteGameObject(iGameObject* go);


	void doDeferredActions();

	static cDebugRenderer* pDebugRenderer;
	static bool debugMode;

private:
	cWorld();
	
	std::set<iGameObject*> deferred_deletes;
	std::set<iGameObject*> deferred_adds;

	static cWorld _world;
};