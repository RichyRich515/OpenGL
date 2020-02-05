#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include "iComponent.hpp"
#include "cMesh.hpp"

class cMeshComponent : public iComponent
{
public:
	// Inherited via iComponent
	virtual void init() override;
	virtual void update(float dt, float tt) override;
	virtual eComponentType getType() override;

	cMesh* mesh; // not necessary??
	std::string meshName;
	glm::mat4 matWorld;
	glm::mat4 inverseTransposeMatWorld;

	float scale;

	void updateMatrices();
};