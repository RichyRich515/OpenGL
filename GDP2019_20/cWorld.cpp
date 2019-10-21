#include "cWorld.hpp"

#include <algorithm>
#include "cBasicShip.hpp"

cWorld cWorld::_world;

sMessage cWorld::message(sMessage const& msg)
{
	if (msg.name == "find stranded")
	{
		sMessage sendmsg = sMessage("Stranded?");
		auto itr = std::find_if(vecGameObjects.begin(), vecGameObjects.end(),
			[sendmsg](cGameObject* go) -> bool { return go->type == "basicShip" && go->message(sendmsg).name == "Yes"; });
		if (itr != vecGameObjects.end())
			return sMessage("new destination", (void*)*itr);
		else
			return sMessage("new desination", (void*)nullptr);
	}
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
