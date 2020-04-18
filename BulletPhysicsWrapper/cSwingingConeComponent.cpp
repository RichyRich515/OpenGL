#include "cSwingingConeComponent.hpp"
#include "nConvert.hpp"

nPhysics::cSwingingConeComponent::cSwingingConeComponent(sSwingingConeDef def)
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
	this->body->setUserPointer(this);
	// Fix eternal swinging
	this->body->setDamping(0.25f, 0.25f);

	this->constraint = new btHingeConstraint(*this->body, btVector3(0.0f, def.Height * 0.5f, 0.0f), btVector3(0.0f, 0.0f, 1.0f));
}

nPhysics::cSwingingConeComponent::~cSwingingConeComponent()
{
	if (this->constraint)
	{
		delete this->constraint;
	}
	if (this->body != nullptr)
	{
		this->body->setUserPointer(nullptr);
		delete this->body->getCollisionShape();
		delete this->body->getMotionState();
		delete this->body;
	}
}

void nPhysics::cSwingingConeComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform;
	this->body->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cSwingingConeComponent::ApplyForce(const glm::vec3& force)
{
	this->body->activate(true);
	this->body->applyCentralForce(nConvert::ToBullet(force));
}
