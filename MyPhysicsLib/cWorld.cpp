#include "cWorld.h"    // My header
#include "nCollide.h"  // collision detection functions from REAL-TIME COLLISION DETECTION, ERICSON
#include <glm/gtx/projection.hpp>

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
		for (cCollisionBody* b : this->mBodies)
		{
			//rb->mAcceleration += this->mGravity;
			this->IntegrateBody(b, dt);
		}

		// 3) Perform collision handling on each unique pair of bodies.
		for (size_t a = 0; a < this->mBodies.size() - 1; ++a)
		{
			for (size_t b = a + 1; b < this->mBodies.size(); ++b)
			{
				if (this->Collide(this->mBodies[a], this->mBodies[b]))
				{
					// TODO: collision listener
				}
			}
		}

		// 4) Clear the acceleration of each rigid body.
		for (cCollisionBody* b : this->mBodies)
		{
			b->ClearAccelerations();
		}
	}

	bool cWorld::AddBody(cCollisionBody* body)
	{
		// 1) Null check
		if (body == nullptr)
			return false;

		// 2) Check if we currently have this rigid body.
		if (std::find(this->mBodies.begin(), this->mBodies.end(), body) != this->mBodies.end())
		{
			// If yes: Return false to indicate nothing was not added.
			return false;
		}
		else
		{
			// If no: Add it, then return true to indicate it was added.
			this->mBodies.push_back(body);
			return true;
		}
	}

	bool cWorld::RemoveBody(cCollisionBody* body)
	{
		// 1) Null check
		if (body == nullptr)
			return false;

		// 2) Check if we currently have this rigid body.
		std::vector<cCollisionBody*>::iterator itr = std::find(this->mBodies.begin(), this->mBodies.end(), body);
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

	void cWorld::IntegrateBody(cCollisionBody* body, float dt)
	{
		// 1) Figure out which body it is
		// 2) pass off to correct integration method
		switch (body->GetBodyType())
		{
		case eBodyType::rigid:
			IntegrateRigidBody(dynamic_cast<cRigidBody*>(body), dt);
			break;
		case eBodyType::soft:
			IntegrateSoftBody(dynamic_cast<cSoftBody*>(body), dt);
			break;
		}
	}

	void cWorld::IntegrateRigidBody(cRigidBody* body, float dt)
	{
		// 1) Static bodies are not to be integrated!
		if (body->IsStatic())
			return;

		body->mAcceleration += this->mGravity;

		// 2) Update the body's previous data.
		body->mPreviousPosition = body->mPosition;
		body->mPreviousVelocity = body->mVelocity;

		// 3) Do some integrating!
		this->mIntegrator.Euler(body->mPosition, body->mVelocity, body->mAcceleration, dt, 0.95f);
	}

	void cWorld::IntegrateSoftBody(cSoftBody* body, float dt)
	{
		size_t numNodes = body->mNodes.size();
		size_t numSprings = body->mSprings.size();

		// 2) accumulate spring forces
		for (size_t i = 0; i < numSprings; ++i)
		{
			body->mSprings[i]->UpdateSpringForce();
			body->mSprings[i]->ApplyForceToNodes();
		}

		// 3) integrate each node, ignore fixed nodes
		for (size_t i = 0; i < numNodes; ++i)
		{
			// Add gravity
			body->mNodes[i]->Acceleration += mGravity;

			body->mNodes[i]->PreviousPosition = body->mNodes[i]->Position;
			body->mNodes[i]->PreviousVelocity = body->mNodes[i]->Velocity;
			if (!body->mNodes[i]->IsFixed())
			{
				this->mIntegrator.Euler(body->mNodes[i]->Position, body->mNodes[i]->Velocity, body->mNodes[i]->Acceleration, dt, 0.3f);
			}
		}

		// 4) internal collisions
		for (size_t i = 0; i < numNodes; ++i)
		{
			cSoftBody::cNode* nodeA = body->mNodes[i];
			for (size_t j = i + 1; j < numNodes - 1; ++j)
			{
				cSoftBody::cNode* nodeB = body->mNodes[j];

				if (nodeA->IsNeighbour(nodeB))
					continue;

				// From our textbook, REAL-TIME COLLISION DETECTION, ERICSON
				float t = 0.0f;
				int res = nCollide::intersect_moving_sphere_sphere(
					nodeA->PreviousPosition, nodeA->Radius, nodeA->Position - nodeA->PreviousPosition,
					nodeB->PreviousPosition, nodeB->Radius, nodeB->Position - nodeB->PreviousPosition,
					t);

				// case A: The spheres don't collide during the timestep.
				//		Return false to indicate no collision happened.
				if (res == 0)
					continue;

				// case B: The spheres were already colliding at the beginning of the timestep.
				//		Do the timestep again for these spheres after applying an impulse that should separate them.
				if (res == -1)
				{
					// 1) Determine the penetration-depth of the spheres at the beginning of the timestep.
					//    (This penetration-depth is the distance the spheres must travel during
					//    the timestep in order to separate.)
					glm::vec3 from_centerA_to_centerB = nodeB->PreviousPosition - nodeA->PreviousPosition;
					float distance = glm::length(from_centerA_to_centerB);

					// 2) Use the sphere's centers to define the direction of our impulse vector.
					glm::vec3 direction = glm::normalize(from_centerA_to_centerB);

					// 3) Use (penetration-depth / DT) to define the magnitude of our impulse vector.
					//    (The impulse vector is now distance/time ...a velocity!)
					float penetration_depth = nodeA->Radius + nodeB->Radius - distance;
					glm::vec3 impulse = direction * penetration_depth / this->mDt;


					// 6) Reset the spheres' positions.
					nodeA->Position = nodeA->PreviousPosition;
					nodeA->Velocity = nodeA->PreviousVelocity;
					nodeB->Position = nodeB->PreviousPosition;
					nodeB->Velocity = nodeB->PreviousVelocity;

					if (nodeA->IsFixed())
					{
						// only apply to node B
						nodeB->Velocity += impulse;
						this->mIntegrator.Euler(nodeB->Position, nodeB->Velocity, nodeB->Acceleration, dt, 0.3f);
						
					}
					else if (nodeB->IsFixed())
					{
						nodeA->Velocity += -impulse;
						this->mIntegrator.Euler(nodeA->Position, nodeA->Velocity, nodeA->Acceleration, dt, 0.3f);
					}
					else
					{
						// two fixed nodes can never collide
						nodeA->Velocity += -impulse / 2.0f;
						nodeB->Velocity += impulse / 2.0f;
						this->mIntegrator.Euler(nodeA->Position, nodeA->Velocity, nodeA->Acceleration, dt, 0.3f);
						this->mIntegrator.Euler(nodeB->Position, nodeB->Velocity, nodeB->Acceleration, dt, 0.3f);
					}
				}

				// case C: The spheres collided during the timestep.
				if (res == 1)
				{
					// 1) Use the outputs from the Ericson function to determine and set the spheres positions to the point of impact.
					float time_to_collision = t * this->mDt;
					nodeA->Position = nodeA->PreviousPosition;
					nodeA->Velocity = nodeA->PreviousVelocity;
					nodeB->Position = nodeB->PreviousPosition;
					nodeB->Velocity = nodeB->PreviousVelocity;
					this->mIntegrator.Euler(nodeA->Position, nodeA->Velocity, nodeA->Acceleration, time_to_collision, 0.3f);
					this->mIntegrator.Euler(nodeB->Position, nodeB->Velocity, nodeB->Acceleration, time_to_collision, 0.3f);

					// 2) Use the inelastic collision response equations from Wikepedia to set they're velocities post-impact.
					float c = 0.50f;
					float ma = nodeA->Mass;
					float mb = nodeB->Mass;
					float mt = ma + mb;
					glm::vec3 vA = nodeA->Velocity;
					glm::vec3 vB = nodeB->Velocity;

					// 3) Re-integrate the spheres with their new velocities over the remaining portion of the timestep.
					float remaining_time = this->mDt - time_to_collision;
					if (nodeA->IsFixed())
					{
						// only apply to node B
						nodeB->Velocity = (c * ma * (vA - vB) + mb * vB + ma * vA) / mt;
						this->mIntegrator.Euler(nodeB->Position, nodeB->Velocity, nodeB->Acceleration, remaining_time, 0.3f);
					}
					else if (nodeB->IsFixed())
					{
						nodeA->Velocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
						this->mIntegrator.Euler(nodeA->Position, nodeA->Velocity, nodeA->Acceleration, remaining_time, 0.3f);
					}
					else
					{
						// two fixed nodes can never collide
						nodeA->Velocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
						nodeB->Velocity = (c * ma * (vA - vB) + mb * vB + ma * vA) / mt;
						this->mIntegrator.Euler(nodeA->Position, nodeA->Velocity, nodeA->Acceleration, remaining_time, 0.3f);
						this->mIntegrator.Euler(nodeB->Position, nodeB->Velocity, nodeB->Acceleration, remaining_time, 0.3f);
					}
				}
			}
		}

		// 5) recalculate for broad phase
		body->mCenter.x = 0;
		body->mCenter.y = 0;
		body->mCenter.z = 0;
		for (size_t i = 0; i < numNodes; ++i)
		{
			body->mCenter += body->mNodes[i]->Position;
		}
		body->mCenter /= numNodes;

		body->mRadius = 0.0f;
		for (size_t i = 0; i < numNodes; ++i)
		{
			float dist = glm::distance(body->mCenter, body->mNodes[i]->Position);
			if (dist > body->mRadius)
				body->mRadius = dist;
		}
		body->mRadius += body->mNodes[0]->Radius;
	}

	bool cWorld::Collide(cCollisionBody* bodyA, cCollisionBody* bodyB)
	{
		// 1) Based on body type, determine which specific collision handling method to use.
		// 2) Cast up the bodies, call the method and return
		switch (bodyA->GetBodyType())
		{
		case eBodyType::rigid:
			switch (bodyB->GetBodyType())
			{
			case eBodyType::rigid:
				return CollideRigidRigid(dynamic_cast<cRigidBody*>(bodyA), dynamic_cast<cRigidBody*>(bodyB));
			case eBodyType::soft:
				return CollideRigidSoft(dynamic_cast<cRigidBody*>(bodyA), dynamic_cast<cSoftBody*>(bodyB));
			default:
				return false;
			}
		case eBodyType::soft:
			switch (bodyB->GetBodyType())
			{
			case eBodyType::rigid:
				return CollideRigidSoft(dynamic_cast<cRigidBody*>(bodyB), dynamic_cast<cSoftBody*>(bodyA));
			case eBodyType::soft:
				// Don't collide soft bodies
				return false;
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool cWorld::CollideRigidRigid(cRigidBody* bodyA, cRigidBody* bodyB)
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
				return CollideSphereSphere(bodyA, (cSphere*)bodyA->GetShape(), bodyB, (cSphere*)bodyB->GetShape());
			default:
				return false;
			}
		default:
			return false;
		}
	}

	bool cWorld::CollideRigidSoft(cRigidBody* bodyA, cSoftBody* bodyB)
	{
		// 1) Based on shape type, determine which specific collision handling method to use.
		// 2) Cast up the shapes, call the methods, return the result.
		switch (bodyA->GetShapeType())
		{
		case eShapeType::plane:
			return CollidePlaneSoft(bodyA, (cPlane*)bodyA->GetShape(), bodyB);
		case eShapeType::sphere:
			return CollideSphereSoft(bodyA, (cSphere*)bodyA->GetShape(), bodyB);
		default:
			return false;
		}
	}

	bool cWorld::CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape)
	{
		// From our textbook, REAL-TIME COLLISION DETECTION, ERICSON
		float t = 0.0f;
		glm::vec3 q(0.0f);
		int result = nCollide::intersect_moving_sphere_plane(sphereBody->mPreviousPosition, sphereShape->GetRadius(), sphereBody->mPosition - sphereBody->mPreviousPosition, planeShape->GetNormal(), planeShape->GetConstant(), t, q);

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
			glm::vec3 impulse = direction * (penetration_depth / this->mDt);

			// 5) Reset the sphere position.
			sphereBody->mPosition = sphereBody->mPreviousPosition;
			sphereBody->mVelocity = sphereBody->mPreviousVelocity;

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
			glm::vec3 projected = glm::proj(sphereBody->mVelocity, planeShape->GetNormal());
			sphereBody->mVelocity -= projected * (1.0f - sphereBody->mElasticity);

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
		int res = nCollide::intersect_moving_sphere_sphere(
			bodyA->mPreviousPosition, shapeA->GetRadius(), bodyA->mPosition - bodyA->mPreviousPosition, 
			bodyB->mPreviousPosition, shapeB->GetRadius(), bodyB->mPosition - bodyB->mPreviousPosition,
			t);

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

	bool cWorld::CollidePlaneSoft(cRigidBody* plane, cPlane* planeShape, cSoftBody* softBody)
	{
		// 1) Check broad phase
		float t;
		glm::vec3 point_of_contact;
		if (nCollide::intersect_moving_sphere_plane(softBody->mCenter, softBody->mRadius, glm::vec3(0.0f), planeShape->GetNormal(), planeShape->GetConstant(), t, point_of_contact))
		{
			bool collided = false;
			size_t numNodes = softBody->NumNodes();
			// 2) Hit broad phase, check each node individually
			for (size_t i = 0; i < numNodes; ++i)
			{
				cSoftBody::cNode* node = softBody->mNodes[i];
				// From our textbook, REAL-TIME COLLISION DETECTION, ERICSON
				t = 0.0f;
				glm::vec3 q(0.0f);
				int result = nCollide::intersect_moving_sphere_plane(node->PreviousPosition, node->Radius, node->Position - node->PreviousPosition, planeShape->GetNormal(), planeShape->GetConstant(), t, q);

				// case A: The sphere did not collide during the timestep.
				//		Return false to indicate no collision happened.
				if (result == 0)
					continue;

				// case B: The sphere was already colliding at the beginning of the timestep.
				//		Do the timestep again for this sphere after applying an impulse that should separate it from the plane.
				if (result == -1)
				{
					// 1) From our textbook, use closest_point_on_plane(..inputs..) to determine the 
					//    penetration-depth of the sphere at the beginning of the timestep.
					//    (This penetration-depth is the distance the sphere must travel during
					//    the timestep in order to escape the plane.)
					glm::vec3 closest_point = nCollide::closest_point_on_plane(node->Position, planeShape->GetNormal(), planeShape->GetConstant());

					// 2) Use the sphere's center and the closest point on the plane to define the direction of our impulse vector.
					glm::vec3 from_point_to_center = node->Position - closest_point;
					glm::vec3 direction = glm::normalize(from_point_to_center);

					// 3) Use (penetration-depth / DT) to define the magnitude of our impulse vector. (The impulse vector is now distance/time ...a velocity!)
					float penetration_depth = node->Radius - glm::length(from_point_to_center);
					glm::vec3 impulse = direction * (penetration_depth / this->mDt);

					// 5) Reset the sphere position.
					node->Position = node->PreviousPosition;
					node->Velocity = node->PreviousVelocity;

					// 4) Apply the impulse vector to sphere velocity.
					node->Velocity += impulse;

					// 6) Re-do the integration for the timestep.
					this->mIntegrator.Euler(node->Position, node->Velocity, node->Acceleration, this->mDt, 0.3f);

					// 7) Return true to indicate a collision has happened.
					collided = true;
				}

				// case C: The sphere collided during the timestep.
				//		Rewind to the point of impact, reflect, then replay.
				if (result == 1)
				{
					node->Position = node->PreviousPosition;
					node->Velocity = node->PreviousVelocity;

					// 1) Use the outputs from the Ericson function to determine and set the sphere position to the point of impact.
					float time_to_collision = t * this->mDt;

					this->mIntegrator.Euler(node->Position, node->Velocity, node->Acceleration, time_to_collision, 0.3f);

					// 2) Reflect the sphere's velocity about the plane's normal vector.
					node->Velocity = glm::reflect(node->Velocity, planeShape->GetNormal());

					// 3) Apply energy loss in direction of normal
					glm::vec3 projected = glm::proj(node->Velocity, planeShape->GetNormal());
					node->Velocity -= projected * 1.0f;

					// 4) Re-integrate the sphere with its new velocity over the remaining portion of the timestep.
					this->mIntegrator.Euler(node->Position, node->Velocity, node->Acceleration, this->mDt - time_to_collision, 0.3f);

					// 5) Return true to indicate a collision has happened.
					collided = true;
				}
			}
			return collided;
		}
		else
		{
			return false;
		}
	}

	bool cWorld::CollideSphereSoft(cRigidBody* sphere, cSphere* sphereShape, cSoftBody* softBody)
	{
		// 1) Check broad phase
		float t;
		if (nCollide::intersect_moving_sphere_sphere(softBody->mCenter, softBody->mRadius, glm::vec3(0.0f), sphere->mPosition, sphereShape->GetRadius(), sphere->mPosition - sphere->mPreviousPosition, t))
		{
			bool collided = false;
			size_t numNodes = softBody->NumNodes();
			// 2) Hit broad phase, check each node individually
			for (size_t i = 0; i < numNodes; ++i)
			{
				cSoftBody::cNode* node = softBody->mNodes[i];

				if (node->IsFixed())
					continue;

				// From our textbook, REAL-TIME COLLISION DETECTION, ERICSON
				t = 0.0f;
				int res = nCollide::intersect_moving_sphere_sphere(
					node->PreviousPosition, node->Radius, node->Position - node->PreviousPosition,
					sphere->mPreviousPosition, sphereShape->GetRadius(), sphere->mPosition - sphere->mPreviousPosition,
					t);

				// case A: The spheres don't collide during the timestep.
				//		Return false to indicate no collision happened.
				if (res == 0)
					continue;

				// case B & C: The spheres collided during the timestep or at the beginning of timestep.
				if (res == 1 || res == -1)
				{
					// 1) Use the outputs from the Ericson function to determine and set the spheres positions to the point of impact.
					float time_to_collision = t * this->mDt;
					node->Position = node->PreviousPosition;
					node->Velocity = node->PreviousVelocity;
					sphere->mPosition = sphere->mPreviousPosition;
					sphere->mVelocity = sphere->mPreviousVelocity;
					this->mIntegrator.Euler(node->Position, node->Velocity, node->Acceleration, time_to_collision, 0.3f);
					this->mIntegrator.Euler(sphere->mPosition, sphere->mVelocity, sphere->mAcceleration, time_to_collision, 0.90f);

					// 2) Use the inelastic collision response equations from Wikepedia to set they're velocities post-impact.
					float c = 0.0f;
					float ma = 0.0f;
					float mb = sphere->mMass;
					float mt = ma + mb;
					glm::vec3 vA = node->Velocity;
					glm::vec3 vB = sphere->mVelocity;

					float remaining_time = this->mDt - time_to_collision;

					// 3) Re-integrate the spheres with their new velocities over the remaining portion of the timestep.
					node->Velocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
					this->mIntegrator.Euler(node->Position, node->Velocity, node->Acceleration, remaining_time, 0.3f);
					this->mIntegrator.Euler(sphere->mPosition, sphere->mVelocity, sphere->mAcceleration, remaining_time, 0.90f);

					collided = true;
				}
			}
			return collided;
		}
		else
		{
			return false;
		}
	}
}