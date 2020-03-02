#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

#include <json/json.h>

#include "iGameObject.hpp"
#include "cGraphicsComponent.hpp"
#include "cMeshComponent.hpp"
#include "cTransformComponent.hpp"
#include "cCoordinatorComponent.hpp"

class cBoidGameObject : public iGameObject
{
public:
	cBoidGameObject();
	cBoidGameObject(std::string name);
	cBoidGameObject(Json::Value& obj);
	cBoidGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual ~cBoidGameObject();

	virtual eComponentType getType() override;

	virtual glm::vec3 getPosition();

	virtual void init();

	virtual void preFrame() override;
	virtual void update(float dt, float tt);
	virtual void render() override;

	virtual sMessage message(sMessage const& msg);

	// Constructor will call this
	virtual void instatiateBaseVariables(const Json::Value& obj) override;
	virtual void instatiateUniqueVariables(const Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;
	virtual void serializeUniqueVariables(Json::Value& obj) override;

	cMeshComponent mesh;
	cGraphicsComponent graphics; 
	cTransformComponent transform;

	cCoordinatorComponent* coordinator;

	float radius;
	float max_speed;
	glm::vec3 velocity;

	float neighbourhood_radius;
};
