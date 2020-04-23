#pragma once

#include "cGameObject.hpp"
#include "cSkinMeshComponent.hpp"
#include <iCharacterComponent.h>

class cAnimatedGameObject : public iGameObject
{
public:
	cAnimatedGameObject();
	virtual ~cAnimatedGameObject();

	virtual eComponentType getType() override;

	virtual sMessage message(sMessage const& msg) override;

	virtual void init() override;
	virtual void preFrame(float dt, float tt) override;
	virtual void update(float dt, float tt) override;
	virtual void render(float dt, float tt) override;
	virtual glm::vec3 getPosition() override;

	cSkinMeshComponent skinmesh;
	cMeshComponent mesh;
	cGraphicsComponent graphics;
	cTransformComponent transform;
	nPhysics::iCharacterComponent* physics;

	float action_timer = 0.0f;
	bool falling;
};