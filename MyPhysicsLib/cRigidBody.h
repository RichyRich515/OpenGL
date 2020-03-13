#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "iShape.h"
#include "cCollisionBody.h"

namespace phys
{
	// sRigidBodyDef
	// 
	// Contains all non-shape related information
	// necessary to create a cRigidBody instance.
	struct sRigidBodyDef
	{
		// Default Constructor
		// Initializes everything to 0
		sRigidBodyDef(): Mass(0.0f), Position(glm::vec3(0.0f)), Velocity(glm::vec3(0.0f)), Elasticity(0.0f) {}

		// Must be set to a non-negative value.
		// 0.0f will result in a static rigid body.
		// A positive value will result in a dynamic rigid body.
		float Mass;

		// The initial position of the rigid body.
		glm::vec3 Position;

		// The initial linear velocity of the rigid body.
		glm::vec3 Velocity;

		// How much it bounces off for a collision, 0 = no bounce, 1.0 = full bounce
		float Elasticity; 
	};

	// cRigidBody
	//
	// Contains all values required for a rigid body simulation.
	// 
	// Shapes may be shared between rigid bodies.
	// Does not own the iShape* used to create it.
	// Will not delete the iShape* it contains when it is deleted.
	class cRigidBody : public cCollisionBody
	{
		// cWorld will be operating on cRigidBody values quite a bit
		// We will trust it to do everything correctly.
		friend class cWorld;
	public:

		// All internal values are initialized according to the sRigidBodyDef
		// A valid iShape* is expected.
		cRigidBody(const sRigidBodyDef& def, iShape* shape);

		// Destructor
		// Will not delete the contained iShape.
		virtual ~cRigidBody();

		// Retrieve a rendering-friendly form of position + rotation
		void GetTransform(glm::mat4& transformOut);

		// Apply an external force to this rigid body.
		// The force is applied center-mass, affecting the acceleration.
		void ApplyForce(const glm::vec3& force);

		// Apply an impulse to the center-mass of this rigid body.
		void ApplyImpulse(const glm::vec3& impulse);

		// A static rigid body was created with a mass of 0.f.
		// A static rigid body does react to other rigid bodies in the world.
		inline bool IsStatic() { return mMass == 0.f; }

		// Get this rigid body's shape.
		// Each rigid body must be created with a shape.
		inline iShape* GetShape() { return mShape; }

		// Get the type of shape of this rigid body's shape.
		// Equivalent to GetShape()->GetShapeType()
		inline eShapeType GetShapeType() { return mShape->GetShapeType(); }

	private:

		// My shape, expected to be valid.
		// cRigidBody will not delete mShape in its destructor.
		iShape* mShape;

		// During a timestep, the previous position is stored for collision related purposes.
		glm::vec3 mPreviousPosition;

		// Current linear position.
		glm::vec3 mPosition;

		// Current linear velocity.
		glm::vec3 mVelocity;

		// During a timestep, the previous velocity is stored for collision related purposes.
		glm::vec3 mPreviousVelocity;

		// Current linear acceleration.
		glm::vec3 mAcceleration;

		// Expected to be non-negative.
		// A value of 0 infers a static rigid body.
		float mMass;

		// Stored for convenience.
		// If mMass is 0, mInvMass will also be 0.
		float mInvMass;


		// Expected to be in range 0 to 1
		// A value of 0 means no elasticity, 1 is full elasticity
		float mElasticity;

		// Constructors not to be used.
		cRigidBody() = delete;
		cRigidBody(const cRigidBody& other) = delete;
		cRigidBody& operator=(const cRigidBody& other) = delete;

		// Inherited via iCollisionBody
		virtual void ClearAccelerations() override;
	};
}