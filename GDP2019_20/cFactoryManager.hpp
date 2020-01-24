#pragma once

#include <string>
#include <map>

#include "iGameObjectFactory.hpp"
#include "cGameObjectFactory.hpp"

class cFactoryManager
{
private:
	static std::map<std::string, iGameObjectFactory*> _mapFactories;
public:
	static iGameObjectFactory* getObjectFactory(std::string type);

	static void cleanUp();
};