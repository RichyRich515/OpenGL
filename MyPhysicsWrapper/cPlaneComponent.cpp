#include "cPlaneComponent.hpp"

nPhysics::cPlaneComponent::cPlaneComponent(sPlaneDef def)
{
	// TODO: dont handle own shape?
	this->shape = new phys::cPlane(def.Normal, def.Constant);
	phys::sRigidBodyDef bodydef;
	bodydef.Mass = 0.0f;
	bodydef.Position = glm::vec3(0.0f);
	bodydef.Velocity = glm::vec3(0.0f);
	this->body = new phys::cRigidBody(bodydef, this->shape);
}

void nPhysics::cPlaneComponent::GetTransform(glm::mat4& transformOut)
{
	return this->body->GetTransform(transformOut);
}
