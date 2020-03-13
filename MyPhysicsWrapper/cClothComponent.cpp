#include "cClothComponent.hpp"

namespace nPhysics
{
	cClothComponent::cClothComponent(sClothDef def) : 
		body(nullptr)
	{
		size_t numNodes = def.NumNodesAcross * def.NumNodesDown;
		phys::sSoftBodyDef sbdef;
		sbdef.Nodes.resize(numNodes);
		sbdef.SpringConstant = def.SpringConstant;

		size_t idxNode = 0;

		glm::vec3 sepAcross(def.CornerB - def.CornerA);
		sepAcross /= (float)(def.NumNodesAcross - 1);

		glm::vec3 sepDown(glm::normalize(def.DownDirection));
		sepDown *= glm::length(sepAcross);

		// Set the node information
		for (size_t x = 0; x < def.NumNodesAcross; ++x)
		{
			for (size_t y = 0; y < def.NumNodesDown; ++y)
			{
				sbdef.Nodes[idxNode].Position = def.CornerA + sepAcross * (float)x + sepDown * (float)y;
				sbdef.Nodes[idxNode].Mass = def.NodeMass;
				++idxNode;
			}
		}

		// Set the spring information

		// All but bottom row and right column
		for (size_t y = 0; y < def.NumNodesDown - 1; ++y)
		{
			for (size_t x = 0; x < def.NumNodesAcross - 1; ++x)
			{
				idxNode = (y * def.NumNodesAcross) + x;
				// set the across spring going right
				sbdef.Springs.push_back(std::make_pair(idxNode, idxNode + 1));
				// set the down spring going down
				sbdef.Springs.push_back(std::make_pair(idxNode, idxNode + def.NumNodesAcross));

			}
		}

		// Set the bottom row springs
		size_t idxBottomRowStart = def.NumNodesAcross * (def.NumNodesDown - 1);
		for (size_t x = 0; x < def.NumNodesAcross - 1; ++x)
		{
			sbdef.Springs.push_back(std::make_pair(idxBottomRowStart + x, idxBottomRowStart + x + 1));
		}

		// Set the right column springs
		for (size_t y = 0; y < def.NumNodesDown - 1; ++y)
		{
			idxNode = def.NumNodesAcross * (y + 1) - 1;
			sbdef.Springs.push_back(std::make_pair(idxNode, idxNode + def.NumNodesAcross));
		}

		this->body = new phys::cSoftBody(sbdef);
	}

	cClothComponent::~cClothComponent()
	{
		if (this->body != nullptr)
			delete this->body;
	}

	void cClothComponent::GetTransform(glm::mat4& transformOut)
	{
		transformOut = glm::mat4(1.0f);
	}

	void cClothComponent::ApplyForce(const glm::vec3& force)
	{
		// todo: ?
	}

	std::size_t cClothComponent::NumNodes()
	{
		return this->body->NumNodes();
	}

	bool cClothComponent::GetNodeRadius(std::size_t index, float& radiusOut)
	{
		return this->body->GetNodeRadius(index, radiusOut);
	}

	bool cClothComponent::GetNodePosition(std::size_t index, glm::vec3& positionOut)
	{
		return this->body->GetNodePosition(index, positionOut);
	}
}
