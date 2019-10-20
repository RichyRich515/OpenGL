#include "cFactoryManager.hpp"

iGameObjectFactory* cFactoryManager::getObjectFactory(std::string type)
{
	if (type == "Basic")
		return new cGameObjectFactory();
	else if (type == "SpaceGame")
		return new cSpaceGameFactory();
	else
		return nullptr;
}
