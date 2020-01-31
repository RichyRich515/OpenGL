#include "cRigidBody.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

namespace phys
{
	cRigidBody::cRigidBody(const sRigidBodyDef& def, iShape* shape) :
		mShape(shape),
		mPreviousPosition(def.Position),
		mPosition(def.Position),
		mVelocity(def.Velocity),
		mAcceleration(glm::vec3(0.0f)),
		mMass(def.Mass)
	{
		if (this->mMass == 0.0f)
		{
			// If mass is 0.0f then rigidbody is static
			this->mInvMass = 0.0f;
		}
		else
		{
			// Dynamic rigidbody
			this->mInvMass = 1.0f / mMass;
		}
	}

	cRigidBody::~cRigidBody()
	{
		// TODO: Is there any clean-up required here?
	}

	void cRigidBody::GetTransform(glm::mat4& transformOut)
	{
		transformOut = glm::mat4(1.0f);
		transformOut *= glm::translate(glm::mat4(1.0f), this->mPosition);
		// TODO: rotation
		//transformOut *= rotation;

		// TODO: scale?
		//transformOut *= scale;
	}

	void cRigidBody::ApplyForce(const glm::vec3& force)
	{
		// force = mass * acceleration
		// so 
		// acceleration = force / mass
		this->mAcceleration += force * this->mInvMass;
	}

	void cRigidBody::ApplyImpulse(const glm::vec3& impulse)
	{
		this->mVelocity += impulse;
	}
}
