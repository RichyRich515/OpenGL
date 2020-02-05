#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <json/json.h>
#include "iComponent.hpp"
#include "cTexture.hpp"


class cGraphicsComponent : public iComponent
{
public:
	static const unsigned MAX_TEXTURES = 6;

	cGraphicsComponent();
	cGraphicsComponent(const Json::Value& obj);
	virtual ~cGraphicsComponent();

	virtual void init() override;
	virtual void update(float dt, float tt) override;
	virtual eComponentType getType() override;

	std::string meshName;

	glm::vec4 color;
	glm::vec4 specular; // RGB is color A is shininess, from 1 to 10000+

	bool visible;
	bool lighting;
	bool wireFrame;

	// TODO: pointers?
	cTexture textures[MAX_TEXTURES];
	cTexture heightmap;
	cTexture discardmap;
};