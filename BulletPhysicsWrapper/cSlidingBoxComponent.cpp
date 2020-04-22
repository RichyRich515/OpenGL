#include "cSlidingBoxComponent.hpp"
#include "nConvert.hpp"

nPhysics::cSlidingBoxComponent::cSlidingBoxComponent(sSlidingBoxDef def)
{
	btCollisionShape* shape = new btBoxShape(nConvert::ToBullet(def.Extents / 2.0f));

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
	this->body->setUserPointer(this);

	btTransform t1;
	t1.setIdentity();
	t1.setRotation(btQuaternion(0.0f, 1.0f, 0.0f, 1.0f));

	this->constraint = new btSliderConstraint(*this->body, t1, false);
	this->constraint->setLowerLinLimit(-275.0f + def.Extents.z * 0.5f);
	this->constraint->setUpperLinLimit(25.0f - def.Extents.z * 0.5f);
}

nPhysics::cSlidingBoxComponent::~cSlidingBoxComponent()
{
	if (this->constraint)
		delete this->constraint;

	if (this->body != nullptr)
	{
		this->body->setUserPointer(nullptr);
		delete this->body->getCollisionShape();
		delete this->body->getMotionState();
		delete this->body;
	}
}

void nPhysics::cSlidingBoxComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform;
	this->body->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cSlidingBoxComponent::ApplyForce(const glm::vec3& force)
{
	this->body->activate(true);
	this->body->applyCentralForce(nConvert::ToBullet(force));
}

int nPhysics::cSlidingBoxComponent::getID()
{
	return this->body->getUserIndex();
}

void nPhysics::cSlidingBoxComponent::setID(int id)
{
	this->body->setUserIndex(id);
}
