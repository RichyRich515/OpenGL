#include "cTransformComponent.hpp"

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


