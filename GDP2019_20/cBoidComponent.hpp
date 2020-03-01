#pragma once

#include "iComponent.hpp"
#include "cTransformComponent.hpp"

class cBoidComponent : public iComponent
{
public:
	virtual void init() override;
	virtual eComponentType getType() override;
	virtual void preFrame() override;
	virtual void update(float dt, float tt) override;
	virtual void render() override;

	cTransformComponent* parentTransform;
};