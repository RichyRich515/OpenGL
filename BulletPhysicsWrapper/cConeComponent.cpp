#include "cConeComponent.hpp"
#include "nConvert.hpp"

nPhysics::cConeComponent::cConeComponent(sConeDef def)
{
	btCollisionShape* shape = new btConeShape(def.Radius, def.Height);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(nConvert::ToBullet(def.Position));

	btScalar mass(def.Mass);

	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	rbInfo.m_restitution = def.Elasticity;
	this->body = new btRigidBody(rbInfo);
	// fix eternal spinning
	this->body->setFriction(0.4f);
	this->body->setRollingFriction(0.4f);
	this->body->setSpinningFriction(0.4f);
	this->body->setUserPointer(this);
}

nPhysics::cConeComponent::~cConeComponent()
{
	if (this->body != nullptr)
	{
		this->body->setUserPointer(nullptr);
		delete this->body->getCollisionShape();
		delete this->body->getMotionState();
		delete this->body;
	}
}

void nPhysics::cConeComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform;
	this->body->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cConeComponent::ApplyForce(const glm::vec3& force)
{
	this->body->activate(true);
	this->body->applyCentralForce(nConvert::ToBullet(force));
}

int nPhysics::cConeComponent::getID()
{
	return this->body->getUserIndex();
}

void nPhysics::cConeComponent::setID(int id)
{
	return this->body->setUserIndex(id);
}
