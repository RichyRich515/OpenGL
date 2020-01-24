#pragma once

#include "cGameObject.hpp"
#include "cCamera.hpp"

class cKakodemon : public cGameObject
{
private:
	// todo behaviour map?
	glm::vec3 targetPosition;
	cCamera* camera;

public:
	virtual void update(float dt, float tt) override;
};