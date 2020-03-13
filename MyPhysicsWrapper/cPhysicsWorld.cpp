#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cClothComponent.hpp"

nPhysics::cPhysicsWorld::cPhysicsWorld()
{
	world = new phys::cWorld();
	// TODO: this someplace else
	world->SetGravity(glm::vec3(0.0f, -9.8f, 0.0f));
}

nPhysics::cPhysicsWorld::~cPhysicsWorld()
{
	if (this->world != nullptr)
		delete this->world;
}

void nPhysics::cPhysicsWorld::Update(float dt)
{
	this->world->Update(dt);
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component)
{
	switch (component->GetComponentType())
	{
	case ePhysicsComponentType::ball:
		return this->world->AddBody(reinterpret_cast<cBallComponent*>(component)->body);
	case ePhysicsComponentType::plane:
		return this->world->AddBody(reinterpret_cast<cPlaneComponent*>(component)->body);
	case ePhysicsComponentType::cloth:
		return this->world->AddBody(reinterpret_cast<cClothComponent*>(component)->body);
	default:
		return false;
	}
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
{
	switch (component->GetComponentType())
	{
	case ePhysicsComponentType::ball:
		return this->world->AddBody(reinterpret_cast<cBallComponent*>(component)->body);
	case ePhysicsComponentType::plane:
		return this->world->AddBody(reinterpret_cast<cPlaneComponent*>(component)->body);
	case ePhysicsComponentType::cloth:
		return this->world->AddBody(reinterpret_cast<cClothComponent*>(component)->body);
	default:
		return false;
	}
}
