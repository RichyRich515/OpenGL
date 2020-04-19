#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cBoxComponent.hpp"
#include "cSlidingBoxComponent.hpp"
#include "cConeComponent.hpp"
#include "cSwingingConeComponent.hpp"
#include "cCylinderComponent.hpp"
#include "cFreeSwingingCylinderComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cTriggerSphereComponent.hpp"
#include "cCharacterComponent.hpp"
#include "bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
#include <iPhysicsComponent.h>
#include <iostream>

// from https://rdmilligan.wordpress.com/2018/01/31/bullet-physics-collision-detection/
void WorldTickCallback(btDynamicsWorld* world, btScalar timestep)
{
	std::vector<nPhysics::iPhysicsComponent*>* listeners = (std::vector<nPhysics::iPhysicsComponent*>*)world->getWorldUserInfo();
	if (listeners == nullptr)
		return;

	int numManifolds = world->getDispatcher()->getNumManifolds();

	for (int i = numManifolds - 1; i >= 0; --i)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();
		nPhysics::iPhysicsComponent* userA = (nPhysics::iPhysicsComponent*)obA->getUserPointer();
		nPhysics::iPhysicsComponent* userB = (nPhysics::iPhysicsComponent*)obB->getUserPointer();

		for (auto listener : *listeners)
		{
			if (listener == userA)
			{
				listener->OnCollision(userB);
			}
			else if (listener == userB)
			{
				listener->OnCollision(userA);
			}
		}
	}
}

nPhysics::cPhysicsWorld::cPhysicsWorld()
{
	this->collisionConfiguration = new btDefaultCollisionConfiguration();

	this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);

	this->broadphase = new btDbvtBroadphase();
	this->ghostPairCallback = new btGhostPairCallback();
	this->broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(this->ghostPairCallback);

	this->solver = new btSequentialImpulseConstraintSolver();

	this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->broadphase, this->solver, this->collisionConfiguration);

	// TODO: from def?
	this->dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
	this->dynamicsWorld->setInternalTickCallback(&WorldTickCallback);
	this->dynamicsWorld->setWorldUserInfo(&(this->collision_listeners));
}

nPhysics::cPhysicsWorld::~cPhysicsWorld()
{
	if (this->dynamicsWorld)
		delete this->dynamicsWorld;
	if (this->solver)
		delete this->solver;
	if (this->ghostPairCallback)
		delete this->ghostPairCallback;
	if (this->broadphase)
		delete this->broadphase;
	if (this->dispatcher)
		delete this->dispatcher;
	if (this->collisionConfiguration)
		delete this->collisionConfiguration;
}

void nPhysics::cPhysicsWorld::Update(float dt)
{
	this->dynamicsWorld->stepSimulation(dt, 100);
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component)
{
	switch (component->GetComponentType())
	{
	case ePhysicsComponentType::ball:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cBallComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::box:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cBoxComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::sliding_box:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cSlidingBoxComponent*>(component)->body);
		this->dynamicsWorld->addConstraint(reinterpret_cast<cSlidingBoxComponent*>(component)->constraint);
		return true;
	case ePhysicsComponentType::cylinder:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cCylinderComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::free_swinging_cylinder:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cFreeSwingingCylinderComponent*>(component)->body);
		this->dynamicsWorld->addConstraint(reinterpret_cast<cFreeSwingingCylinderComponent*>(component)->constraint);
		return true;
	case ePhysicsComponentType::cone:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cConeComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::swinging_cone:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cSwingingConeComponent*>(component)->body);
		this->dynamicsWorld->addConstraint(reinterpret_cast<cSwingingConeComponent*>(component)->constraint);
		return true;
	case ePhysicsComponentType::plane:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cPlaneComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::trigger_sphere:
		this->dynamicsWorld->addCollisionObject(reinterpret_cast<cTriggerSphereComponent*>(component)->ghostObject);
		return true;
	case ePhysicsComponentType::character:
		// add ghost object separately
		this->dynamicsWorld->addCollisionObject(reinterpret_cast<cCharacterComponent*>(component)->ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
		this->dynamicsWorld->addCharacter(reinterpret_cast<cCharacterComponent*>(component)->character);
		return true;
	default:
		return false;
	}
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
{
	if (component == nullptr)
		return false;

	switch (component->GetComponentType())
	{
	case ePhysicsComponentType::ball:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cBallComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::box:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cBoxComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::sliding_box:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cSlidingBoxComponent*>(component)->body);
		this->dynamicsWorld->removeConstraint(reinterpret_cast<cSlidingBoxComponent*>(component)->constraint);
		return true;
	case ePhysicsComponentType::cylinder:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cCylinderComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::free_swinging_cylinder:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cFreeSwingingCylinderComponent*>(component)->body);
		this->dynamicsWorld->removeConstraint(reinterpret_cast<cFreeSwingingCylinderComponent*>(component)->constraint);
		return true;
	case ePhysicsComponentType::cone:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cConeComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::swinging_cone:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cSwingingConeComponent*>(component)->body);
		this->dynamicsWorld->removeConstraint(reinterpret_cast<cSwingingConeComponent*>(component)->constraint);
		return true;
	case ePhysicsComponentType::plane:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cPlaneComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::trigger_sphere:
		this->dynamicsWorld->removeCollisionObject(reinterpret_cast<cTriggerSphereComponent*>(component)->ghostObject);
		return true;
	case ePhysicsComponentType::character:
		this->dynamicsWorld->removeCollisionObject(reinterpret_cast<cCharacterComponent*>(component)->ghostObject);
		this->dynamicsWorld->removeCharacter(reinterpret_cast<cCharacterComponent*>(component)->character);
		return true;
	default:
		return false;
	}
}

bool nPhysics::cPhysicsWorld::AddCollisionListener(iPhysicsComponent* component)
{
	if (component == nullptr)
		return false;

	auto itr = std::find(this->collision_listeners.begin(), this->collision_listeners.end(), component);
	if (itr == this->collision_listeners.end())
	{
		this->collision_listeners.push_back(component);
		return true;
	}
	return false;
}

bool nPhysics::cPhysicsWorld::RemoveCollisionListener(iPhysicsComponent* component)
{
	if (component == nullptr)
		return false;

	auto itr = std::find(this->collision_listeners.begin(), this->collision_listeners.end(), component);
	if (itr != this->collision_listeners.end())
	{
		this->collision_listeners.erase(itr);
		return true;
	}
	return false;
}
