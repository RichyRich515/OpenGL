#pragma once
#include <glm/vec3.hpp>   // single include header for all glm math stuffs

namespace phys
{
	// cIntegrator
	// 
	// A single place for numerical integration schemes.
	//
	// Contains single entry-points for each supported integrations scheme.
	class cIntegrator
	{
	private:

	public:

		cIntegrator();


		// EULER ALGORITHM

		// pos : object's position in space
		// vel : velocity of object
		// acc : acceleration of object
		// dt : delta time (time since last step)
		void Euler(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt, float damp = 1.0f);

		// pos : object's position in space
		// vel : velocity of object
		// acc : acceleration of object
		// appliedAcc : impulse forces applied to object (player inputs, wind fields, etc.)
		// dt : delta time (time since last step)
		void Euler(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt);


		// FOURTH-ORDER RUNGE-KUTTA

		// pos : object's position in space
		// vel : velocity of object
		// acc : acceleration of object
		// dt : delta time (time since last step)
		void RK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt);

		// pos : object's position in space
		// vel : velocity of object
		// acc : acceleration of object
		// appliedAcc : impulse forces applied to object (player inputs, wind fields, etc.)
		// dt : delta time (time since last step)
		void RK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt);



		// TODO: implement these maybe
		//// EULER-CROMER ALGORITHM
		//void EulerCromer(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt);
		//void EulerCromer(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt);
		//// MIDPOINT ALGORITHM
		//void MidPoint(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt);
		//void MidPoint(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt);
		//// VELOCITY-VERLET ALGORITHM <- with half-step acceleration lag
		//void VelocityVerlet(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt);
		//void VelocityVerlet(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt);
	};
}