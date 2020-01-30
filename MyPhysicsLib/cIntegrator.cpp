#include "cIntegrator.h"  // My header

namespace phys
{
	cIntegrator::cIntegrator()
	{

	}

	void cIntegrator::Euler(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt)
	{
		vel.x += acc.x * dt;
		vel.y += acc.y * dt;
		vel.y += acc.z * dt;

		pos.x += vel.x * dt;
		pos.y += vel.y * dt;
		pos.y += vel.z * dt;
	}

	void cIntegrator::Euler(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt)
	{
		// TODO: confirm this?
		acc += appliedAcc; 

		vel.x += acc.x * dt;
		vel.y += acc.y * dt;
		vel.y += acc.z * dt;

		pos.x += vel.x * dt;
		pos.y += vel.y * dt;
		pos.y += vel.z * dt;
	}

	// FOURTH-ORDER RUNGE-KUTTA
	void cIntegrator::RK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt)
	{
		// TODO: math!
		// For an excellent article on RK4 implementation:
		// https://gafferongames.com/post/integration_basics/
	}

	// FOURTH-ORDER RUNGE-KUTTA
	void cIntegrator::RK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt)
	{
		// TODO: math!
		// For an excellent article on RK4 implementation:
		// https://gafferongames.com/post/integration_basics/
	}


	//// EULER-CROMER ALGORITHM
	//void cIntegrator::EulerCromer(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt)
	//{
	//	// TODO: math!
	//}

	//void cIntegrator::EulerCromer(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt)
	//{
	//	// TODO: math!
	//}

	//// MIDPOINT ALGORITHM
	//void cIntegrator::MidPoint(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt)
	//{
	//	// TODO: math!
	//}

	//void cIntegrator::MidPoint(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt)
	//{
	//	// TODO: math!
	//}

	//// VELOCITY-VERLET ALGORITHM <- with half-step acceleration lag
	//void cIntegrator::VelocityVerlet(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, float dt)
	//{
	//	// TODO: math!
	//}

	//void cIntegrator::VelocityVerlet(glm::vec3& pos, glm::vec3& vel, glm::vec3& acc, const glm::vec3& appliedAcc, float dt)
	//{
	//	// TODO: math!
	//}
}