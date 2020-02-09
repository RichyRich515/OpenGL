#include "cPhysicsManager.hpp"

#include <iostream>
#include <Windows.h>


nPhysics::iPhysicsFactory* cPhysicsManager::physicsFactory;
nPhysics::iPhysicsWorld* cPhysicsManager::physicsWorld;
HMODULE cPhysicsManager::hModule;

cPhysicsManager::cPhysicsManager(const char* physics_library_name)
{
	hModule = NULL;
	physicsFactory = nullptr;
	physicsWorld = nullptr;
	{
		hModule = LoadLibraryA(physics_library_name);
		if (!hModule)
		{
			std::cout << "Error loading physics library: " << physics_library_name << std::endl;
			return;
		}

		// make a Physics factory
		physicsFactory = ((func_createPhysicsFactory*)GetProcAddress(hModule, physics_interface_factory_func_name))();
		if (physicsFactory == nullptr)
		{
			FreeLibrary(hModule);
			hModule = NULL;
			std::cout << "Error creating physics factory: " << physics_library_name << std::endl;
			return;
		}
	}

	physicsWorld = physicsFactory->CreateWorld();
	if (physicsWorld == nullptr)
	{
		FreeLibrary(hModule);
		hModule = NULL;
		std::cout << "Error creating physics world: " << physics_library_name << std::endl;
		return;
	}
}

cPhysicsManager::~cPhysicsManager()
{
	if (physicsFactory != nullptr)
		delete physicsFactory;

	if (physicsWorld != nullptr)
		delete physicsWorld;
	
	if (hModule)
		FreeLibrary(hModule);
}

nPhysics::iPhysicsFactory* cPhysicsManager::getFactory()
{
	return physicsFactory;
}

nPhysics::iPhysicsWorld* cPhysicsManager::getWorld()
{
	return physicsWorld;
}
