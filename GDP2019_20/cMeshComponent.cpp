#include "cMeshComponent.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

void cMeshComponent::init()
{
}

void cMeshComponent::update(float dt, float tt)
{
}

eComponentType cMeshComponent::getType()
{
	return eComponentType();
}

void cMeshComponent::updateMatrices()
{
	this->matWorld = glm::mat4(1.0f);
	this->matWorld *= glm::translate(glm::mat4(1.0f), this->position);
	this->matWorld *= glm::mat4(this->orientation);
	this->matWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(this->scale, this->scale, this->scale));
	this->inverseTransposeMatWorld = glm::inverse(glm::transpose(this->matWorld));
}
