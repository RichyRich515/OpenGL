#include "cCharacterComponent.hpp"
#include "nConvert.hpp"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

nPhysics::cCharacterComponent::cCharacterComponent(sCharacterDef def)
{
	this->ghostObject = new btPairCachingGhostObject();
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(nConvert::ToBullet(def.Position));
	this->ghostObject->setWorldTransform(transform);

	btConvexShape* shape = new btCapsuleShapeZ(def.Radius, def.Height);
	this->ghostObject->setCollisionShape(shape);
	this->ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	// TODO: stuff into def
	this->character = new btKinematicCharacterController(this->ghostObject, shape, 1.0f, btVector3(0, 1, 0));
	this->character->setJumpSpeed(def.JumpSpeed);
}

nPhysics::cCharacterComponent::~cCharacterComponent()
{
	if (this->character)
		delete this->character;
}

void nPhysics::cCharacterComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform = this->character->getGhostObject()->getWorldTransform();
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cCharacterComponent::ApplyForce(const glm::vec3& force)
{
	this->character->applyImpulse(nConvert::ToBullet(force));
}

void nPhysics::cCharacterComponent::Jump()
{
	this->character->jump();
}

bool nPhysics::cCharacterComponent::CanJump()
{
	return this->character->canJump();
}

void nPhysics::cCharacterComponent::GetVelocity(glm::vec3& v)
{
	nConvert::ToSimple(this->character->getLinearVelocity(), v);
}

int nPhysics::cCharacterComponent::getID()
{
	return this->ghostObject->getUserIndex();
}

void nPhysics::cCharacterComponent::setID(int id)
{
	this->ghostObject->setUserIndex(id);
}

void nPhysics::cCharacterComponent::Move(glm::vec3 v, float time)
{
	this->character->setLinearVelocity(nConvert::ToBullet(v));
}
