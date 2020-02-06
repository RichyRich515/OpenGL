#include "cGraphicsComponent.hpp"
#include "JsonHelper.hpp"

cGraphicsComponent::cGraphicsComponent() :
	meshName(""),
	visible(true),
	lighting(false),
	wireFrame(false),
	color(glm::vec4(1.0f)),
	specular(glm::vec4(1.0f))
{
}

cGraphicsComponent::cGraphicsComponent(const Json::Value& obj)
{
	meshName = obj["meshName"] ? obj["meshName"].asString() : "";
	visible = obj["visible"] ? obj["visible"].asBool() : true;
	lighting = obj["lighting"] ? obj["lighting"].asBool() : false;
	wireFrame = obj["wireFrame"] ? obj["wireFrame"].asBool() : false;
	color = obj["color"] ? Json::toVec4(obj["color"]) : glm::vec4(1.0f);
	specular = obj["specular"] ? Json::toVec4(obj["specular"]) : glm::vec4(1.0f);
}

cGraphicsComponent::~cGraphicsComponent()
{
}

void cGraphicsComponent::init()
{
}

void cGraphicsComponent::preFrame()
{
}

void cGraphicsComponent::update(float dt, float tt)
{
}

void cGraphicsComponent::render()
{
}

eComponentType cGraphicsComponent::getType()
{
	return eComponentType::Graphics;
}