#pragma once

#include <vector>
#include <set>

#include "iMessageable.hpp"
#include "iGameObjectFactory.hpp"
#include "cGameObject.hpp"
#include "cCamera.hpp"
#include "DebugRenderer/cDebugRenderer.h"
#include "cLight.hpp"

// Singleton world object
class cWorld : iMessageable
{
public:
	virtual sMessage message(sMessage const& msg);

	void addGameObject(cGameObject* go);
	void addLight(cLight* light);

	static cWorld* getWorld();

	std::vector<cGameObject*> vecGameObjects;
	std::vector<cLight*> vecLights;

	iGameObjectFactory* gameObjectFactory;

	void deferredAddGameObject(cGameObject* go);
	void deferredDeleteGameObject(cGameObject* go);


	void doDeferredActions();

	static cDebugRenderer* pDebugRenderer;
	static cCamera* pCamera;
	static bool debugMode;

private:
	cWorld();
	
	std::set<cGameObject*> deferred_deletes;
	std::set<cGameObject*> deferred_adds;

	static cWorld _world;
};