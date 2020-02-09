#pragma once

#include <json/json.h>

enum class eComponentType
{
	None = 0,
	GameObject,
	Graphics,
	Physics,
	Transform,
	Mesh
};

class iComponent
{
public:
	virtual ~iComponent() {}
	virtual void init() = 0;

	virtual eComponentType getType() = 0;
	
	virtual void preFrame() = 0;
	virtual void update(float dt, float tt) = 0;
	virtual void render() = 0;
	
	virtual void instatiateBaseVariables(const Json::Value& obj) {}
	virtual void instatiateUniqueVariables(const Json::Value& obj) {}

	virtual void serializeJSONObject(Json::Value& obj) {}
	virtual void serializeUniqueVariables(Json::Value& obj) {}
};