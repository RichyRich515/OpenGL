#include "cCoordinatorComponent.hpp"
#include "cBoidGameObject.hpp"
#include "cSteeringFunctions.hpp"

void cCoordinatorComponent::init()
{
}

eComponentType cCoordinatorComponent::getType()
{
	return eComponentType();
}

void cCoordinatorComponent::preFrame()
{
}

void cCoordinatorComponent::update(float dt, float tt)
{
	if (this->path_follow)
	{
		glm::vec3 destination(this->path_nodes[current_node]);
		glm::vec3 steer(0.0f);

		steer = cSteeringFunctions::steerSeekArrive(this->position, destination, this->velocity, 4.0f, this->max_speed, dt);

		glm::vec3 old_velocity = velocity;
		if (glm::length(steer) > 0.0f)
			this->velocity += steer;

		this->position += this->velocity * dt;
		if (glm::length(this->velocity) > 0.0f && glm::distance(this->position, destination) > 0.25f)
		{
			this->orientation = glm::slerp(this->orientation, glm::quatLookAt(glm::normalize(-this->velocity), glm::vec3(0.0f, 1.0f, 0.0f)), dt * 5.0f);
			for (unsigned i = 0; i < this->offsets.size(); ++i)
			{
				this->offsets[i] = this->orientation * this->unmodified_offsets[i];
			}
		}
		else
		{
			this->current_node += this->path_dir;
			if (this->current_node >= (int)this->path_nodes.size())
				this->current_node = 0;
			else if (this->current_node < 0)
				this->current_node = this->path_nodes.size() - 1;
		}
	}
	
	// Check for collisions
	for (unsigned i = 0; i < boids.size() - 1; ++i)
	{
		for (unsigned j = i + 1; j < boids.size(); ++j)
		{
			float dist = glm::distance(boids[i]->transform.position, boids[j]->transform.position);
			float rr = boids[i]->radius + boids[j]->radius;
			float overlap = rr - dist;
			if (overlap > 0.0f)
			{
				// collision
				glm::vec3 diff = boids[j]->transform.position - boids[i]->transform.position;
				glm::vec3 dir = glm::normalize(diff);

				float iratio = boids[i]->radius / rr;
				float jratio = 1 - iratio;

				boids[i]->transform.position += -dir * overlap * iratio;
				boids[j]->transform.position += dir * overlap * jratio;
			}
		}
	}
}

void cCoordinatorComponent::render()
{
}

glm::vec3 cCoordinatorComponent::flock(unsigned idx, float neighbourhood_radius, float dt)
{
	glm::vec3 sep(0.0f);
	glm::vec3 align(0.0f);
	glm::vec3 coh(0.0f);

	float separationRadius = 10.0f;
	int neighbourCount = 0;
	for (int i = 0; i < boids.size(); i++) 
	{
		if (i == idx)
			continue;

		float dist = glm::distance(boids[idx]->transform.position, boids[i]->transform.position);
		if ((dist > 0) && (dist < separationRadius)) 
		{
			neighbourCount++;

			sep += glm::normalize(boids[idx]->transform.position - boids[i]->transform.position) / dist;
			align += boids[i]->velocity;
			coh += boids[i]->transform.position;
		}
	}

	if (neighbourCount > 0)
	{
		sep = glm::normalize(sep / (float)neighbourCount) * boids[idx]->max_speed - boids[idx]->velocity;
		align = glm::normalize(align / (float)neighbourCount) * boids[idx]->max_speed - boids[idx]->velocity;
		coh /= (float)neighbourCount;
		coh = cSteeringFunctions::steerSeekArrive(boids[idx]->transform.position, coh, boids[idx]->velocity, 0.0f, boids[idx]->max_speed, dt);
	}

	sep *= this->weights.x;
	align *= this->weights.y;
	coh *= this->weights.z;
	glm::vec3 path = glm::vec3(0.0f);
	if (this->path_follow)
	{
		path = cSteeringFunctions::steerSeekArrive(boids[idx]->transform.position, this->path_nodes[this->current_node], boids[idx]->velocity, 0.0f, boids[idx]->max_speed, dt) * this->weights.z * 0.5f;
		coh *= 0.5f;
	}
		
	return sep + align + coh + path;
}
