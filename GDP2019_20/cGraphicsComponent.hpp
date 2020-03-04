#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <json/json.h>
#include "iComponent.hpp"
#include "cTexture.hpp"
#include "cShaderManager.hpp"


class cGraphicsComponent : public iComponent
{
public:
	static const unsigned MAX_TEXTURES = 6;

	cGraphicsComponent();
	cGraphicsComponent(const Json::Value& obj);
	virtual ~cGraphicsComponent();

	virtual void init() override;
	virtual void preFrame(float dt, float tt) override;
	virtual void update(float dt, float tt) override;
	virtual void render(float dt, float tt) override;
	virtual eComponentType getType() override;

	virtual void instatiateBaseVariables(const Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;

	glm::vec4 color;
	glm::vec4 specular; // RGB is color A is shininess, from 1 to 10000+

	bool visible;
	bool lighting;
	bool wireFrame;

	bool reflects;
	bool refracts;

	// TODO: pass these in render???
	float dt;
	float tt;


	// TODO: pointers?
	cTexture textures[MAX_TEXTURES];
	cTexture heightmap;
	cTexture discardmap;

	cShaderManager::cShaderProgram* pShader;
};