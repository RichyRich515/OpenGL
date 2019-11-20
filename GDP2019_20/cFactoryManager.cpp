#include "cFactoryManager.hpp"

iGameObjectFactory* cFactoryManager::getObjectFactory(std::string type)
{
	if (type == "Basic")
		return new cGameObjectFactory();
	else
		return nullptr;
}
