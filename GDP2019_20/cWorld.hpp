#pragma once

#include <vector>

#include "iMessageable.hpp"
#include "cGameObject.hpp"

// Singleton world object
class cWorld : iMessageable
{
public:
	virtual sMessage message(sMessage const& msg);

	void addGameObject(cGameObject* go);

	static cWorld* getWorld();
private:
	cWorld();

	std::vector<cGameObject*> vecGameObjects;

	static cWorld _world;
};