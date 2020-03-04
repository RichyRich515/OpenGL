#pragma once

#include "cGameObject.hpp"
#include "cSkinMeshComponent.hpp"

class cAnimatedGameObject : public iGameObject
{
public:
	cAnimatedGameObject();
	cAnimatedGameObject(std::string name);
	cAnimatedGameObject(Json::Value& obj);
	cAnimatedGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);

	virtual eComponentType getType() override;

	virtual sMessage message(sMessage const& msg) override;

	virtual void init() override;
	virtual void preFrame() override;
	virtual void update(float dt, float tt) override;
	virtual void render() override;
	virtual glm::vec3 getPosition() override;

	cSkinMeshComponent skinmesh;
	cMeshComponent mesh;
	cGraphicsComponent graphics;
	cTransformComponent transform;

	glm::vec3 velocity;
	glm::quat forward;

	float action_timer = 0.0f;

	bool active = false;
	bool jumping = false;
	bool falling = false;
	bool punching = false;

	bool alive = true;

	float jump_timer;
	float jump_start_y;
	float jump_speed;

	int dir = -1;

	std::vector<std::vector<int>>* level;
	std::vector<iGameObject*> robots;
};