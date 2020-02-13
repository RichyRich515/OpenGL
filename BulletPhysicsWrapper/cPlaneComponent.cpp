#include "cPlaneComponent.hpp"
#include "nConvert.hpp"

nPhysics::cPlaneComponent::cPlaneComponent(sPlaneDef def)
{
	btCollisionShape* shape = new btStaticPlaneShape(nConvert::ToBullet(def.Normal), def.Constant);

	btTransform transform;
	transform.setIdentity();

	// TODO: ????
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, shape, btVector3(0, 0, 0));
	rbInfo.m_restitution = 0.5f;
	this->body = new btRigidBody(rbInfo);
	this->body->setUserPointer(this);
}

nPhysics::cPlaneComponent::~cPlaneComponent()
{
	if (this->body != nullptr)
	{
		this->body->setUserPointer(nullptr);
		delete this->body->getCollisionShape();
		delete this->body->getMotionState();
		delete this->body;
	}
}

void nPhysics::cPlaneComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform;
	this->body->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cPlaneComponent::ApplyForce(const glm::vec3& force)
{
	// do nothing
}
