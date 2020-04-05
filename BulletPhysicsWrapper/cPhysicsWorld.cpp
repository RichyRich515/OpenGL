#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cBoxComponent.hpp"
#include "cConeComponent.hpp"
#include "cCylinderComponent.hpp"
#include "cPlaneComponent.hpp"

nPhysics::cPhysicsWorld::cPhysicsWorld()
{
	this->collisionConfiguration = new btDefaultCollisionConfiguration();

	this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);

	this->overlappingPairCache = new btDbvtBroadphase();

	this->solver = new btSequentialImpulseConstraintSolver();

	this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->overlappingPairCache, this->solver, this->collisionConfiguration);

	// TODO: from def
	this->dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
}

nPhysics::cPhysicsWorld::~cPhysicsWorld()
{
	if (this->dynamicsWorld)
		delete this->dynamicsWorld;
	if (this->solver)
		delete this->solver;
	if (this->overlappingPairCache)
		delete this->overlappingPairCache;
	if (this->dispatcher)
		delete this->dispatcher;
	if (this->collisionConfiguration)
		delete this->collisionConfiguration;
}

void nPhysics::cPhysicsWorld::Update(float dt)
{
	this->dynamicsWorld->stepSimulation(dt, 10);

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
	case ePhysicsComponentType::cylinder:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cCylinderComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::cone:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cConeComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::plane:
		this->dynamicsWorld->addRigidBody(reinterpret_cast<cPlaneComponent*>(component)->body);
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
	case ePhysicsComponentType::cylinder:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cCylinderComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::cone:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cConeComponent*>(component)->body);
		return true;
	case ePhysicsComponentType::plane:
		this->dynamicsWorld->removeRigidBody(reinterpret_cast<cPlaneComponent*>(component)->body);
		return true;
	default:
		return false;
	}
}