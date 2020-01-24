#include "cFactoryManager.hpp"


std::map<std::string, iGameObjectFactory*> cFactoryManager::_mapFactories;

iGameObjectFactory* cFactoryManager::getObjectFactory(std::string type)
{
	if (_mapFactories[type])
	{
		return _mapFactories[type];
	}
	else if (type == "Basic")
	{
		return new cGameObjectFactory();
	}
	else
	{
		return nullptr;
	}
}

void cFactoryManager::cleanUp()
{
	for (auto f : _mapFactories)
		delete f.second;
}
