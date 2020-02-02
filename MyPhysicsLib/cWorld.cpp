#include "cWorld.h"    // My header
#include "nCollide.h"  // collision detection functions from
// REAL-TIME COLLISION DETECTION, ERICSON

namespace phys
{
	cWorld::cWorld() :
		mDt(0.0f),
		mGravity(glm::vec3(0.0f))
	{

	}

	cWorld::~cWorld()
	{
		// TODO: Is there anything in your world that needs cleaning up?
	}

	void cWorld::GetGravity(glm::vec3& gravityOut)
	{
		gravityOut = mGravity;
	}

	void cWorld::SetGravity(const glm::vec3& gravityIn)
	{
		mGravity = gravityIn;
	}

	void cWorld::Update(float dt)
	{
		// 1) If we have no bodies, there's nothing to do... return.
		if (this->mBodies.size() == 0)
			return;

		this->mDt = dt;

		// 2) Integrate each body.
		for (cRigidBody* rb : this->mBodies)
		{
			rb->mAcceleration += this->mGravity;
			this->IntegrateRigidBody(rb, dt);
		}

		// 3) Perform collision handling on each unique pair of bodies.
		for (size_t a = 0; a < this->mBodies.size() - 1; ++a)
		{
			for (size_t b = a + 1; b < this->mBodies.size(); ++b)
			{
				this->Collide(this->mBodies[a], this->mBodies[b]);
			}
		}

		// 4) Clear the acceleration of each rigid body.
		for (cRigidBody* rb : this->mBodies)
		{
			rb->mAcceleration.x = 0.0f;
			rb->mAcceleration.y = 0.0f;
			rb->mAcceleration.z = 0.0f;
		}
	}

	bool cWorld::AddRigidBody(cRigidBody* rigidBody)
	{
		// 1) Null check
		if (rigidBody == nullptr)
			return false;

		// 2) Check if we currently have this rigid body.
		if (std::find(this->mBodies.begin(), this->mBodies.end(), rigidBody) != this->mBodies.end())
		{
			// If yes: Return false to indicate nothing was not added.
			return false;
		}
		else
		{
			// If no: Add it, then return true to indicate it was added.
			this->mBodies.push_back(rigidBody);
			return true;
		}
	}

	bool cWorld::RemoveRigidBody(cRigidBody* rigidBody)
	{
		// 1) Null check
		if (rigidBody == nullptr)
			return false;

		// 2) Check if we currently have this rigid body.
		std::vector<cRigidBody*>::iterator itr = std::find(this->mBodies.begin(), this->mBodies.end(), rigidBody);
		if (itr != this->mBodies.end())
		{
			// If yes: remove it, then return true to indicate it was removed.
			this->mBodies.erase(itr);
			return true;
		}
		else
		{
			// If no: Return false to indicate nothing was removed.
			return false;
		}
	}

	void cWorld::IntegrateRigidBody(cRigidBody* body, float dt)
	{
		// 1) Static bodies are not to be integrated!
		if (body->IsStatic())
			return;

		// 2) Update the body's previous data.
		body->mPreviousPosition = body->mPosition;
		body->mPreviousVelocity = body->mVelocity;

		// TODO: RK4
		// 3) Do some integrating!
		this->mIntegrator.Euler(body->mPosition, body->mVelocity, body->mAcceleration, dt);

		// 4) Apply some linear damping
		body->mVelocity *= powf(0.95f, dt);
	}

	bool cWorld::Collide(cRigidBody* bodyA, cRigidBody* bodyB)
	{
		// 1) Based on shape type, determine which specific collision handling method to use.
		// 2) Cast up the shapes, call the methods, return the result.
		switch (bodyA->GetShapeType())
		{
		case eShapeType::plane:
			switch (bodyB->GetShapeType())
			{
			case eShapeType::plane:
				// Do not collide planes
				return false;
			case eShapeType::sphere:
				return CollideSpherePlane(bodyB, (cSphere*)bodyB->GetShape(), bodyA, (cPlane*)bodyA->GetShape());
			default:
				return false;
			}
		case eShapeType::sphere:
			switch (bodyB->GetShapeType())
			{
			case eShapeType::plane:
				return CollideSpherePlane(bodyA, (cSphere*)bodyA->GetShape(), bodyB, (cPlane*)bodyB->GetShape());
			case eShapeType::sphere:
				return CollideSphereSphere(bodyA, (cSphere*)bodyB->GetShape(), bodyB, (cSphere*)bodyB->GetShape());
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool cWorld::CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape)
	{
		// From our textbook, REAL-TIME COLLISION DETECTION, ERICSON
		float t = 0.0f;
		glm::vec3 q(0.0f);
		int result = nCollide::intersect_moving_sphere_plane(sphereBody->mPosition, sphereShape->GetRadius(), sphereBody->mPosition - sphereBody->mPreviousPosition, planeShape->GetNormal(), planeShape->GetConstant(), t, q);

		// case A: The sphere did not collide during the timestep.
		//		Return false to indicate no collision happened.
		if (result == 0)
		{
			return false;
		}

		// case B: The sphere was already colliding at the beginning of the timestep.
		//		Do the timestep again for this sphere after applying an impulse that should separate it from the plane.
		if (result == -1)
		{
			// 1) From our textbook, use closest_point_on_plane(..inputs..) to determine the 
			//    penetration-depth of the sphere at the beginning of the timestep.
			//    (This penetration-depth is the distance the sphere must travel during
			//    the timestep in order to escape the plane.)
			glm::vec3 closest_point = nCollide::closest_point_on_plane(sphereBody->mPosition, planeShape->GetNormal(), planeShape->GetConstant());

			// 2) Use the sphere's center and the closest point on the plane to define the direction of our impulse vector.
			glm::vec3 from_point_to_center = sphereBody->mPosition - closest_point;
			glm::vec3 direction = glm::normalize(from_point_to_center);

			// 3) Use (penetration-depth / DT) to define the magnitude of our impulse vector. (The impulse vector is now distance/time ...a velocity!)
			float penetration_depth = sphereShape->GetRadius() - glm::length(from_point_to_center);
			glm::vec3 impulse = direction * penetration_depth / this->mDt;

			// 5) Reset the sphere position.
			sphereBody->mPosition = sphereBody->mPreviousPosition;
			sphereBody->mVelocity = sphereBody->mPreviousVelocity;

			// a) Reflect the sphere's velocity about the plane's normal vector.
			sphereBody->mVelocity = glm::reflect(sphereBody->mVelocity, planeShape->GetNormal());

			// b) Apply some energy loss (to the velocity) in the direction of the plane's normal vector.
			// TODO: THIS CORRECTLY
			sphereBody->mVelocity *= sphereBody->mElasticity;

			// 4) Apply the impulse vector to sphere velocity.
			sphereBody->ApplyImpulse(impulse);

			// 6) Re-do the integration for the timestep.
			this->IntegrateRigidBody(sphereBody, this->mDt);

			// 7) Return true to indicate a collision has happened.
			return true;
		}

		// case C: The sphere collided during the timestep.
		//		Rewind to the point of impact, reflect, then replay.
		if (result == 1)
		{
			sphereBody->mPosition = sphereBody->mPreviousPosition;
			sphereBody->mVelocity = sphereBody->mPreviousVelocity;

			// 1) Use the outputs from the Ericson function to determine and set the sphere position to the point of impact.
			float time_to_collision = t * this->mDt;

			this->IntegrateRigidBody(sphereBody, time_to_collision);

			// 2) Reflect the sphere's velocity about the plane's normal vector.
			sphereBody->mVelocity = glm::reflect(sphereBody->mVelocity, planeShape->GetNormal());

			// 3) Apply some energy loss (to the velocity) in the direction of the plane's normal vector.
			// TODO: THIS CORRECTLY
			sphereBody->mVelocity *= sphereBody->mElasticity;

			// 4) Re-integrate the sphere with its new velocity over the remaining portion of the timestep.
			this->IntegrateRigidBody(sphereBody, this->mDt - time_to_collision);

			// 5) Return true to indicate a collision has happened.
			return true;
		}

		return false;
	}

	bool cWorld::CollideSphereSphere(cRigidBody* bodyA, cSphere* shapeA, cRigidBody* bodyB, cSphere* shapeB)
	{
		// From our textbook, REAL-TIME COLLISION DETECTION, ERICSON
		float t = 0.0f;
		int res = nCollide::intersect_moving_sphere_sphere(bodyA->mPosition, shapeA->GetRadius(), bodyA->mPosition - bodyA->mPreviousPosition, bodyB->mPosition, shapeB->GetRadius(), bodyB->mPosition - bodyB->mPreviousPosition, t);

		// case A: The spheres don't collide during the timestep.
		//		Return false to indicate no collision happened.
		if (res == 0)
		{
			return false;
		}

		// case B: The spheres were already colliding at the beginning of the timestep.
		//		Do the timestep again for these spheres after applying an impulse that should separate them.
		if (res == -1)
		{
			// 1) Determine the penetration-depth of the spheres at the beginning of the timestep.
			//    (This penetration-depth is the distance the spheres must travel during
			//    the timestep in order to separate.)
			glm::vec3 from_centerA_to_centerB = bodyB->mPreviousPosition - bodyA->mPreviousPosition;
			float distance = glm::length(from_centerA_to_centerB);

			// 2) Use the sphere's centers to define the direction of our impulse vector.
			glm::vec3 direction = glm::normalize(from_centerA_to_centerB);
			
			// 3) Use (penetration-depth / DT) to define the magnitude of our impulse vector.
			//    (The impulse vector is now distance/time ...a velocity!)
			float penetration_depth = shapeA->GetRadius() + shapeB->GetRadius() - distance;
			glm::vec3 impulse = direction * penetration_depth / this->mDt;


			// 6) Reset the spheres' positions.
			bodyA->mPosition = bodyA->mPreviousPosition;
			bodyA->mVelocity = bodyA->mPreviousVelocity;
			bodyB->mPosition = bodyB->mPreviousPosition;
			bodyB->mVelocity = bodyB->mPreviousVelocity;

			//// 2) Use the inelastic collision response equations from Wikepedia to set they're velocities post-impact.
			//float c = 0.50f;
			/*float c = bodyA->mElasticity * bodyB->mElasticity;
			float ma = bodyA->mMass;
			float mb = bodyB->mMass;
			float mt = ma + mb;
			glm::vec3 vA = bodyA->mVelocity;
			glm::vec3 vB = bodyB->mVelocity;

			bodyA->mVelocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
			bodyB->mVelocity = (c * ma * (vA - vB) + mb * vB + ma * vA) / mt;*/


			// 4) Apply a portion of the impulse vector to sphereA's velocity.
			bodyA->ApplyImpulse(-impulse / 2.0f);

			// 5) Apply a portion of the impulse vector to sphereB's velocity.
			//    (Be sure to apply the impulse in opposing directions.)
			bodyB->ApplyImpulse(impulse / 2.0f);

			// 7) Re-do the integration for the timestep.
			this->IntegrateRigidBody(bodyA, this->mDt);
			this->IntegrateRigidBody(bodyB, this->mDt);

			// 8) Return true to indicate a collision has happened.
			return true;
		}

		// case C: The spheres collided during the timestep.
		if (res == 1)
		{
			// 1) Use the outputs from the Ericson function to determine and set the spheres positions to the point of impact.
			float time_to_collision = t * this->mDt;
			bodyA->mPosition = bodyA->mPreviousPosition;
			bodyA->mVelocity = bodyA->mPreviousVelocity;
			bodyB->mPosition = bodyB->mPreviousPosition;
			bodyB->mVelocity = bodyB->mPreviousVelocity;
			this->IntegrateRigidBody(bodyA, time_to_collision);
			this->IntegrateRigidBody(bodyB, time_to_collision);

			// 2) Use the inelastic collision response equations from Wikepedia to set they're velocities post-impact.
			//float c = 0.50f;
			float c = bodyA->mElasticity * bodyB->mElasticity;
			float ma = bodyA->mMass;
			float mb = bodyB->mMass;
			float mt = ma + mb;
			glm::vec3 vA = bodyA->mVelocity;
			glm::vec3 vB = bodyB->mVelocity;

			bodyA->mVelocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
			bodyB->mVelocity = (c * ma * (vA - vB) + mb * vB + ma * vA) / mt;

			// 3) Re-integrate the spheres with their new velocities over the remaining portion of the timestep.
			float remaining_time = this->mDt - time_to_collision;
			this->IntegrateRigidBody(bodyA, remaining_time);
			this->IntegrateRigidBody(bodyB, remaining_time);

			// 4) Return true to indicate a collision has happened.
			return true;
		}

		return false;
	}
}