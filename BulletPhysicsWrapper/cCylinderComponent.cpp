#include "cCylinderComponent.hpp"
#include "nConvert.hpp"

nPhysics::cCylinderComponent::cCylinderComponent(sCylinderDef def)
{
	btCollisionShape* shape = new btCylinderShape(nConvert::ToBullet(def.Extents / 2.0f));

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
	this->body->setFriction(0.4f);
	this->body->setRollingFriction(0.4f);
	this->body->setSpinningFriction(0.4f);
	this->body->setUserPointer(this);
}

nPhysics::cCylinderComponent::~cCylinderComponent()
{
	if (this->body != nullptr)
	{
		this->body->setUserPointer(nullptr);
		delete this->body->getCollisionShape();
		delete this->body->getMotionState();
		delete this->body;
	}
}

void nPhysics::cCylinderComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform;
	this->body->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cCylinderComponent::ApplyForce(const glm::vec3& force)
{
	this->body->activate(true);
	this->body->applyCentralForce(nConvert::ToBullet(force));
}

int nPhysics::cCylinderComponent::getID()
{
	return this->body->getUserIndex();
}

void nPhysics::cCylinderComponent::setID(int id)
{
	this->body->setUserIndex(id);
}
