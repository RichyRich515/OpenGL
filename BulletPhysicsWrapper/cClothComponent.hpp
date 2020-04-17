#pragma once
#include <iClothComponent.h>

namespace nPhysics
{
	class cClothComponent : public iClothComponent
	{
	private:
		friend class cPhysicsWorld;
		
		// TODO:
		//btSoftBody* body;

	public:
		cClothComponent(sClothDef def);
		virtual ~cClothComponent();

		virtual void GetTransform(glm::mat4& transformOut) override;

		virtual void ApplyForce(const glm::vec3& force) override;

		virtual std::size_t NumNodes() override;
		virtual std::size_t NumNodesAcross() override;
		virtual std::size_t NumNodesDown() override;
		virtual bool GetNodeRadius(std::size_t index, float& radiusOut) override;
		virtual bool GetNodePosition(std::size_t index, glm::vec3& positionOut) override;
	};
}