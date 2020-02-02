#pragma once

#include "cGameObject.hpp"
#include "cCamera.hpp"

class cCacodemon : public cGameObject
{
private:

	enum class eEnemyType
	{
		type_a = 0,
		type_b,
		type_c,
		type_d,
	};

	float maxSpeed = 7.5f;
	eEnemyType enemyType = eEnemyType::type_a;
	
	unsigned currentTexture = 0; // 0 == face, 1 == back
	
	cTexture face_texture;
	cTexture face_discard_texture;
	cTexture back_texture;
	cTexture back_discard_texture;

	bool idling = false; // for wanderer
	float wander_idle_timer = 6.0f;

	float shot_timer = 0.0f;
public:
	cCacodemon(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	cCacodemon() {};
	virtual ~cCacodemon() {};
	virtual void init() override;
	virtual void update(float dt, float tt) override;
};