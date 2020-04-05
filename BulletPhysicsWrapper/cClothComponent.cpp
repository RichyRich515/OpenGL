#include "cClothComponent.hpp"
#include "nConvert.hpp"

namespace nPhysics
{
	cClothComponent::cClothComponent(sClothDef def)
	{
	}

	cClothComponent::~cClothComponent()
	{
	}

	void cClothComponent::GetTransform(glm::mat4& transformOut)
	{
	}

	void cClothComponent::ApplyForce(const glm::vec3& force)
	{
	}

	std::size_t cClothComponent::NumNodes()
	{
		return 0;
	}

	std::size_t cClothComponent::NumNodesAcross()
	{
		return 0;
	}

	std::size_t cClothComponent::NumNodesDown()
	{
		return 0;
	}

	bool cClothComponent::GetNodeRadius(std::size_t index, float& radiusOut)
	{
		return false;
	}

	bool cClothComponent::GetNodePosition(std::size_t index, glm::vec3& positionOut)
	{
		return false;
	}

}
