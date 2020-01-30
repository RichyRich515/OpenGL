#pragma once

#include "cGameObject.hpp"

class cBullet : public cGameObject
{
public:
	bool from_enemy;

	virtual void update(float dt, float tt) override;
};
