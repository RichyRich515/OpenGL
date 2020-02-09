#include "cBallComponent.hpp"

namespace nPhysics
{
	cBallComponent::cBallComponent(sBallDef def)
	{
		// TODO: dont handle own shape?
		this->shape = new phys::cSphere(def.Radius);
		phys::sRigidBodyDef bodydef;
		bodydef.Mass = def.Mass;
		bodydef.Position = def.Position;
		bodydef.Velocity = glm::vec3(0.0f);
		bodydef.Elasticity = def.Elasticity;

		this->body = new phys::cRigidBody(bodydef, this->shape);
	}

	cBallComponent::~cBallComponent()
	{
		if (this->body != nullptr)
			delete this->body;

		if (this->shape != nullptr)
			delete this->shape;
	}

	void cBallComponent::GetTransform(glm::mat4& transformOut)
	{
		this->body->GetTransform(transformOut);
	}
}
