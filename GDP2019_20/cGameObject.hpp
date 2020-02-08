#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

#include <json/json.h>

#include "iMessageable.hpp"

#include "iComponent.hpp"
#include <iPhysicsComponent.h>
#include "cGraphicsComponent.hpp"
#include "cMeshComponent.hpp"

class cGameObject : public iMessageable, public iComponent
{
public:
	unsigned id;

	std::string name;
	std::string type;

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
	virtual void instatiateBaseVariables(Json::Value& obj) override;
	virtual void instatiateUniqueVariables(Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;
	virtual void serializeUniqueVariables(Json::Value& obj) override;

	cMeshComponent mesh;
	cGraphicsComponent graphics;
	nPhysics::iPhysicsComponent* physics;
	
};
