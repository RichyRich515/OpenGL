#pragma once
#include "cRigidBody.h"   // cRigidBody class
#include "cSoftBody.h"   // cRigidBody class
#include <vector>         // std::vector to contain body pointers
#include "shapes.h"       // all specific shape classes
#include "cIntegrator.h"  // cIntegrator class for timesteps

namespace phys
{
	// cWorld
	// 
	// A physics bodies simulator.
	// 
	// Pointers to body instances can be added and removed.
	// 
	// cWorld will operate on the contained bodies during timesteps
	// triggered by calls to cWorld::Update(dt)
	// 
	// cWorld does not own body pointers, and will not delete them when it is deleted.
	class cWorld
	{
	public:

		// Create a cWorld
		// The world is initially empty, containing no bodies.
		cWorld();

		// Does not delete any body pointers.
		~cWorld();

		// Get the gravity acceleration vector.
		void GetGravity(glm::vec3& gravityOut);

		// Set the gravity acceleration vector.
		// This will be applied to all bodies during timesteps.
		void SetGravity(const glm::vec3& gravityIn);

		// A single trigger to perform a single timestep.
		// All bodies will go through integration, collision
		// detection and reaction, and have their internal values updated.
		void Update(float dt);

		// Add a body to the world. 
		// Returns true if:
		//    There was an addition to the world.
		//    This particular body was added to the world.
		//    (It must not have already been contained in the world)
		// Returns false if:
		//    There was no addition to the world.
		//    A null pointer is passed in.
		//    This particular body is already in the world, hense not added again.
		bool AddBody(cCollisionBody* body);
		
		// Remove a body from the world.
		// Returns true if:
		//    There was a removal from the world.
		//    This particular body existed in the world and was removed.
		// Returns false if:
		//    There was no removal from the world.
		//    A null pointer was passed in.
		//    This particular body was not in the world, hense not removed.
		bool RemoveBody(cCollisionBody* body);

	protected:
		// Entry point for a single step of integration
		// Safe for null pointers.
		// Safe for static bodies.
		void IntegrateBody(cCollisionBody* body, float dt);

		// Performs a single numerical integration step.
		// Safe for null pointers.
		// Safe for static bodies.
		void IntegrateRigidBody(cRigidBody* body, float dt);
		
		// Performs a single numerical integration step.
		// Safe for null pointers.
		void IntegrateSoftBody(cSoftBody* body, float dt);

		// Entry point for collision detection.
		// Returns the result of specific methods.
		bool Collide(cCollisionBody* bodyA, cCollisionBody* bodyB);

		// Collision between two rigidbodies
		// Returns the result of specific methods.
		bool CollideRigidRigid(cRigidBody* bodyA, cRigidBody* bodyB);

		// Collision between rigidbody and softbody
		// Returns the result of specific methods.
		bool CollideRigidSoft(cRigidBody* bodyA, cSoftBody* bodyB);

		// Handles collision detection and reaction between a sphere and a plane.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape);

		// Handles collision detection and reaction between two spheres.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSphereSphere(cRigidBody* bodyA, cSphere* shapeA, cRigidBody* bodyB, cSphere* shapeB);

		// Handles collision detection and reaction between plane and soft body.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollidePlaneSoft(cRigidBody* plane, cPlane* planeShape, cSoftBody* softBody);

		// Handles collision detection and reaction between sphere and soft body.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSphereSoft(cRigidBody* sphere, cSphere* sphereShape, cSoftBody* softBody);

	private:

		// The delta time input cached during each Update(dt) call.
		float mDt;

		// Our handy dandy integrator.
		cIntegrator mIntegrator;

		// Acceleration due to gravity applied to each body
		// during each timestep.
		glm::vec3 mGravity;

		// All the bodies currently in the world.
		// Not owned by cWorld, will not be deleted in the destructor.
		std::vector<cCollisionBody*> mBodies;

		// Constructors not to be used.
		cWorld(const cWorld& other) = delete;
		cWorld& operator=(const cWorld& other) = delete;
	};
}