#include "cTriggerSphereComponent.hpp"
#include "nConvert.hpp"

nPhysics::cTriggerSphereComponent::cTriggerSphereComponent(sTriggerSphereDef def)
{
	this->ghostObject = new btPairCachingGhostObject();
	this->ghostObject->setUserPointer(this);
	this->ghostObject->setCollisionShape(new btSphereShape(def.Radius));

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(nConvert::ToBullet(def.Position));

	this->ghostObject->setWorldTransform(transform);
	this->ghostObject->setCollisionFlags(this->ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

nPhysics::cTriggerSphereComponent::~cTriggerSphereComponent()
{
	if (this->ghostObject)
	{
		this->ghostObject->setUserPointer(nullptr);
		delete this->ghostObject->getCollisionShape();
		delete this->ghostObject;
	}
}

bool nPhysics::cTriggerSphereComponent::IsTriggeredBy(int id)
{
	int overlaps = this->ghostObject->getNumOverlappingObjects();
	for (int i = 0; i < overlaps; ++i)
	{
		if (this->ghostObject->getOverlappingObject(i)->getUserIndex() == id)
			return true;
	}
	return false;
}

void nPhysics::cTriggerSphereComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform = this->ghostObject->getWorldTransform();
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cTriggerSphereComponent::ApplyForce(glm::vec3 const& v)
{
	return;
}

int nPhysics::cTriggerSphereComponent::getID()
{
	return this->ghostObject->getUserIndex();
}

void nPhysics::cTriggerSphereComponent::setID(int id)
{
	this->ghostObject->setUserIndex(id);
}
