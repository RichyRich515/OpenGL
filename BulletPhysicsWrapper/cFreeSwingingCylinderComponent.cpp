#include "cFreeSwingingCylinderComponent.hpp"
#include "nConvert.hpp"

nPhysics::cFreeSwingingCylinderComponent::cFreeSwingingCylinderComponent(sFreeSwingingCylinderDef def)
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
	this->body->setUserPointer(this);
	// Fix eternal swinging
	this->body->setDamping(0.05f, 0.05f);

	btTransform t1;
	t1.setIdentity();
	t1.setOrigin(btVector3(0.0f, def.Extents.y / 2.0f, 0.0f)); // set to the top for pivot point
	this->constraint = new btConeTwistConstraint(*this->body, t1);

	// make it based on it's own position, not world 0/0/0
	btTransform t2;
	t2.setIdentity();
	t2.setOrigin(nConvert::ToBullet(def.Position));

	this->constraint->setFrames(t1, t2);
}

nPhysics::cFreeSwingingCylinderComponent::~cFreeSwingingCylinderComponent()
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

void nPhysics::cFreeSwingingCylinderComponent::GetTransform(glm::mat4& transformOut)
{
	btTransform transform;
	this->body->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cFreeSwingingCylinderComponent::ApplyForce(const glm::vec3& force)
{
	this->body->activate(true);
	this->body->applyCentralForce(nConvert::ToBullet(force));
}

int nPhysics::cFreeSwingingCylinderComponent::getID()
{
	return this->body->getUserIndex();
}

void nPhysics::cFreeSwingingCylinderComponent::setID(int id)
{
	this->body->setUserIndex(id);
}
