#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cBoxComponent.hpp"
#include "cSlidingBoxComponent.hpp"
#include "cConeComponent.hpp"
#include "cSwingingConeComponent.hpp"
#include "cCylinderComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cCharacterComponent.hpp"
#include "bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

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
	this->dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
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

	// TODO: collisionlistening
	//if (this->collisionListener)
	//{
	//	
	//}
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
	case ePhysicsComponentType::character:
		this->dynamicsWorld->removeCollisionObject(reinterpret_cast<cCharacterComponent*>(component)->ghostObject);
		this->dynamicsWorld->removeCharacter(reinterpret_cast<cCharacterComponent*>(component)->character);
		return true;
	default:
		return false;
	}
}