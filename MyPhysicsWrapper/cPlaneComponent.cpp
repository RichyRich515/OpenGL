#include "cPlaneComponent.hpp"

nPhysics::cPlaneComponent::cPlaneComponent(sPlaneDef def)
{
	this->shape = new phys::cPlane(def.Normal, def.Constant);
	phys::sRigidBodyDef bodydef;
	bodydef.Mass = 0.0f;
	bodydef.Position = glm::vec3(0.0f);
	bodydef.Velocity = glm::vec3(0.0f);
	this->body = new phys::cRigidBody(bodydef, this->shape);
}

nPhysics::cPlaneComponent::~cPlaneComponent()
{
	if (this->body != nullptr)
		delete this->body;

	if (this->shape != nullptr)
		delete this->shape;
}

void nPhysics::cPlaneComponent::GetTransform(glm::mat4& transformOut)
{
	return this->body->GetTransform(transformOut);
}

void nPhysics::cPlaneComponent::ApplyForce(const glm::vec3& force)
{
	// do nothing
}
