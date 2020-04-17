#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

#include <json/json.h>

#include "iGameObject.hpp"
#include <iPhysicsComponent.h>
#include "cGraphicsComponent.hpp"
#include "cMeshComponent.hpp"

class cBulletGameObject : public iGameObject
{
public:
	cBulletGameObject();
	cBulletGameObject(std::string name);
	cBulletGameObject(Json::Value& obj);
	cBulletGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual ~cBulletGameObject();

	virtual eComponentType getType() override;

	virtual glm::vec3 getPosition();

	virtual void init();

	virtual void preFrame(float dt, float tt) override;
	virtual void update(float dt, float tt);
	virtual void render(float dt, float tt) override;

	virtual sMessage message(sMessage const& msg);

	// Constructor will call this
	virtual void instatiateBaseVariables(const Json::Value& obj) override;
	virtual void instatiateUniqueVariables(const Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;
	virtual void serializeUniqueVariables(Json::Value& obj) override;

	cMeshComponent* mesh;
	cGraphicsComponent graphics;
	nPhysics::iPhysicsComponent* physics;


	float life_time = 2.0f;
};
