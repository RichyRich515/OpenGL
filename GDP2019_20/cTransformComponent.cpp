#include "cTransformComponent.hpp"
#include "JsonHelper.hpp"

void cTransformComponent::setPosition(glm::vec3 const& pos)
{
	this->position = pos;
}

void cTransformComponent::setPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}

glm::vec3 cTransformComponent::getPosition()
{
	return this->position;
}

void cTransformComponent::setOrientation(glm::quat const& q)
{
	this->orientation = q;
}

void cTransformComponent::setOrientation(glm::vec3 const& rotation, bool deg)
{
	this->orientation = glm::quat(deg ? rotation * glm::pi<float>() / 180.0f : rotation);
}

void cTransformComponent::setOrientation(float x, float y, float z, bool deg)
{
	glm::vec3 v(x, y, z);
	this->orientation = glm::quat(deg ? v * glm::pi<float>() / 180.0f : v);
}

glm::quat cTransformComponent::getOrientation()
{
	return this->orientation;
}

void cTransformComponent::translate(glm::vec3 const& translation)
{
	this->position += translation;
}

void cTransformComponent::rotate(glm::quat const& q)
{
	this->orientation *= q;
}

void cTransformComponent::rotate(glm::vec3 const& rotation, bool deg)
{
	this->orientation *= glm::quat(deg ? rotation * glm::pi<float>() / 180.0f : rotation);
}

void cTransformComponent::rotate(float x, float y, float z, bool deg)
{
	glm::vec3 v(x, y, z);
	this->orientation *= glm::quat(deg ? v * glm::pi<float>() / 180.0f : v);
}

void cTransformComponent::init()
{
}

void cTransformComponent::preFrame()
{
}

void cTransformComponent::update(float dt, float tt)
{
}

void cTransformComponent::render()
{
}

eComponentType cTransformComponent::getType()
{
	return eComponentType::Transform;
}

void cTransformComponent::instatiateBaseVariables(const Json::Value& obj)
{
	this->position = obj["position"] ? Json::toVec3(obj["position"]) : glm::vec3(0.0f);
	this->orientation = obj["orientation"] ? Json::toQuat(obj["orientation"]) : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	this->updateMatricis();
}

void cTransformComponent::serializeJSONObject(Json::Value& obj)
{
}

void cTransformComponent::updateMatricis()
{
	this->matWorld = glm::mat4(1.0f);

	this->matWorld *= glm::translate(glm::mat4(1.0f), this->position);
	this->matWorld *= glm::mat4(this->orientation);
	this->inverseTransposeMatWorld = glm::inverse(glm::transpose(this->matWorld));
	//this->matWorld *= this->scale;
}
