#pragma once

#include "iComponent.hpp"
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

class cBoidGameObject;

enum class eBoidBehaviour
{
	formation = 0,
	flock
};

class cCoordinatorComponent : public iComponent
{
public:
	virtual void init() override;
	virtual eComponentType getType() override;
	virtual void preFrame() override;
	virtual void update(float dt, float tt) override;
	virtual void render() override;

	glm::vec3 position;
	std::vector<glm::vec3> offsets;
	std::vector<glm::vec3> unmodified_offsets;
	std::vector<cBoidGameObject*> boids;

	std::vector<glm::vec3> circleOffsets;
	std::vector<glm::vec3> vOffsets;
	std::vector<glm::vec3> squareOffsets;
	std::vector<glm::vec3> lineOffsets;
	std::vector<glm::vec3> twoRowsOffsets;

	float max_speed;
	glm::vec3 velocity;
	glm::quat orientation;

	eBoidBehaviour behaviour;
	bool path_follow;

	int current_node;
	int path_dir;
	std::vector<glm::vec3> path_nodes;

	// x separation
	// y alignment
	// z cohesion
	glm::vec3 weights;

	glm::vec3 flock(unsigned idx, float neighbourhood_radius, float dt);

	float separation_radius;
};