#pragma once

#include "iComponent.hpp"
#include <vector>

#include <glm/vec3.hpp>

class cBoidGameObject;

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
	std::vector<cBoidGameObject*> boids;

	std::vector<glm::vec3> circleOffsets;
	std::vector<glm::vec3> vOffsets;
	std::vector<glm::vec3> squareOffsets;
	std::vector<glm::vec3> lineOffsets;
	std::vector<glm::vec3> twoRowsOffsets;

};