#pragma once

#include <string>
#include <glm/mat4x4.hpp>
#include "iComponent.hpp"
#include "cMesh.hpp"

class cMeshComponent : public iComponent
{
public:
	// Inherited via iComponent
	virtual void init() override;
	virtual void preFrame(float dt, float tt) override;
	virtual void update(float dt, float tt) override;
	virtual void render(float dt, float tt) override;
	virtual eComponentType getType() override;

	virtual void instatiateBaseVariables(const Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;

	cMesh* mesh; // not necessary??
	std::string meshName;

	float scale;
};