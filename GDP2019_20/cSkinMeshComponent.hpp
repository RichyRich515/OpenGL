#pragma once

#include "iComponent.hpp"
#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include <queue>

class cSkinMeshComponent : public iComponent
{
private:
	unsigned MAX_ANIMATION_QUEUE_SIZE = 5;

	std::string default_animation;
	std::string current_animation;

	std::queue<std::pair<std::string, bool>> animation_queue;

	void startNextAnimation();

	float anim_time;
	float total_animation_time;

	bool must_finish_animation;
	bool looping;
public:
	// Inherited via iComponent
	virtual void init() override;
	virtual eComponentType getType() override;
	virtual void preFrame(float dt, float tt) override;
	virtual void update(float dt, float tt) override;
	virtual void render(float dt, float tt) override;

	cSimpleAssimpSkinnedMesh skinmesh;

	void queueAnimation(std::string animationName, bool loop);
	void forceNextAnimation();
	void forceAnimation(std::string animationName, bool loop, bool resetTime = false);
};