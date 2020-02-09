#pragma once

#include "iComponent.hpp"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

class cTransformComponent : public iComponent
{
public:
	// Inherited via iComponent
	virtual void init() override;
	virtual void preFrame() override;
	virtual void update(float dt, float tt) override;
	virtual void render() override;
	virtual eComponentType getType() override;

	virtual void instatiateBaseVariables(const Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;

	glm::vec3 position;
	glm::quat orientation;

	void updateMatricis();

	glm::mat4 matWorld;
	glm::mat4 inverseTransposeMatWorld;

	void setPosition(glm::vec3 const& pos);
	void setPosition(float x, float y, float z);
	glm::vec3 getPosition();

	void setOrientation(glm::quat const& q);
	void setOrientation(glm::vec3 const& rotation, bool deg = false);
	void setOrientation(float x, float y, float z, bool deg = false);
	glm::quat getOrientation();
	
	void translate(glm::vec3 const& translation);

	void rotate(glm::quat const& q);
	void rotate(glm::vec3 const& rotation, bool deg = false);
	void rotate(float x, float y, float z, bool deg = false);
};