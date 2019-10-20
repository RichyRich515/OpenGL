#pragma once

#include <string>

#include "iGameObjectFactory.hpp"
#include "cGameObjectFactory.hpp"
#include "cSpaceGameFactory.hpp"

class cFactoryManager
{
public:
	static iGameObjectFactory* getObjectFactory(std::string type);
};