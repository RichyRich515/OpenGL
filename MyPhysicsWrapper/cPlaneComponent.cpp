#include "cPlaneComponent.hpp"

void nPhysics::cPlaneComponent::GetTransform(glm::mat4& transformOut)
{
	return this->body->GetTransform(transformOut);
}
