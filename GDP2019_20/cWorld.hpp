#pragma once

#include <vector>

#include "iMessageable.hpp"
#include "cGameObject.hpp"
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
private:
	cWorld();

	static cWorld _world;
};