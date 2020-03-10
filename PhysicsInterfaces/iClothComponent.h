#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sClothDef
	{
		glm::vec3 CornerA;
		glm::vec3 CornerB;
		glm::vec3 DownDirection;

		std::size_t NumNodesAcross;
		std::size_t NumNodesDown;

		float NodeMass;
	};

	class iClothComponent : public iPhysicsComponent
	{
	public:
		virtual ~iClothComponent() {}

		virtual std::size_t NumNodes() = 0;
		virtual bool GetNodeRadius(std::size_t index, float& radiusOut) = 0;
		virtual bool GetNodePosition(std::size_t index, glm::vec3& positionOut) = 0;

	protected:
		iClothComponent() : iPhysicsComponent(ePhysicsComponentType::cloth) {}
	private:
		iClothComponent(const iClothComponent& other) = delete;
		iClothComponent& operator=(const iClothComponent& other) = delete;
	};
}