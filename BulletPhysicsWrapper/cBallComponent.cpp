#include "cBallComponent.hpp"
#include "nConvert.hpp"

namespace nPhysics
{
	cBallComponent::cBallComponent(sBallDef def)
	{
		btCollisionShape* shape = new btSphereShape(def.Radius);

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
	}

	cBallComponent::~cBallComponent()
	{
		if (this->body != nullptr)
		{
			this->body->setUserPointer(nullptr);
			delete this->body->getCollisionShape();
			delete this->body->getMotionState();
			delete this->body;
		}
	}

	void cBallComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		this->body->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cBallComponent::ApplyForce(const glm::vec3& force)
	{
		this->body->activate(true);
		this->body->applyCentralForce(nConvert::ToBullet(force));
	}
}
