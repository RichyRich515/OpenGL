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
#include "cTransformComponent.hpp"

class cGameObject : public iGameObject
{
public:
	cGameObject();
	cGameObject(std::string name);
	cGameObject(Json::Value& obj);
	cGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual ~cGameObject();

	virtual eComponentType getType() override;

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
};
