#pragma once

#include <iPhysicsFactory.h>
#include <Windows.h>

// MUST BE DELETED LAST AFTER ANY PHYSICS COMPONENTS
class cPhysicsManager
{
private:
	static nPhysics::iPhysicsFactory* physicsFactory;
	static nPhysics::iPhysicsWorld* physicsWorld;
	static HMODULE hModule;

	static constexpr char physics_interface_factory_func_name[19] = "MakePhysicsFactory";
	typedef nPhysics::iPhysicsFactory* (func_createPhysicsFactory)();

public:
	cPhysicsManager(const char* physics_library_name);
	~cPhysicsManager();

	static nPhysics::iPhysicsFactory* getFactory();
	static nPhysics::iPhysicsWorld* getWorld();
};