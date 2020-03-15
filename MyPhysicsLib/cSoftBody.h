#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "cCollisionBody.h"
#include <vector>

namespace phys
{

	// sSoftBodyNodeDef
	//
	struct sSoftBodyNodeDef
	{
		glm::vec3 Position;
		float Mass;
	};

	// sSoftBodyDef
	//
	struct sSoftBodyDef
	{
		std::vector<sSoftBodyNodeDef> Nodes;
		std::vector<std::pair<size_t, size_t>> Springs;
		float SpringConstant;
	};

	// cSoftBody
	//
	class cSoftBody : public cCollisionBody
	{
		class cSpring;

		class cNode
		{
		public:
			cNode(const sSoftBodyNodeDef& nodeDef);
			void CalculateRadius();
			inline bool IsFixed() { return Mass == 0.0f; }
			bool IsNeighbour(cNode* other);
			void ApplyForce(glm::vec3 force);
			std::vector<cSpring*> Springs;
			float Mass;
			float InvMass;
			float Radius;
			glm::vec3 Position;
			glm::vec3 PreviousPosition;
			glm::vec3 Velocity;
			glm::vec3 PreviousVelocity;
			glm::vec3 Acceleration;
		};

		class cSpring
		{
		public:
			cSpring(cNode* nodeA, cNode* nodeB, float springConstant);
			void UpdateSpringForce();
			void ApplyForceToNodes();
			cNode* GetOther(cNode* node);
			cNode* NodeA;
			cNode* NodeB;
			float SpringConstant;
			float RestingLength;
			glm::vec3 SpringForceAtoB;
		};

		// cWorld will be operating on cSoftBody values quite a bit
		// We will trust it to do everything correctly.
		friend class cWorld;

	public:
		// All internal values are initialized according to the def
		cSoftBody(const sSoftBodyDef& def);

		// Destructor
		virtual ~cSoftBody();

		size_t NumNodes();

		bool GetNodeRadius(size_t index, float& radiusOut);

		bool GetNodePosition(size_t index, glm::vec3& positionOut);

		// Apply a force to each node
		void ApplyForce(glm::vec3 force);

	private:
		// Constructors not to be used.
		cSoftBody() = delete;
		cSoftBody(const cSoftBody& other) = delete;
		cSoftBody& operator=(const cSoftBody& other) = delete;

		std::vector<cNode*> mNodes;
		std::vector<cSpring*> mSprings;

		// Inherited via iCollisionBody
		virtual void ClearAccelerations() override;
		
		// recalculated each frame for broad phase collision detection
		glm::vec3 mCenter;
		float mRadius;
	};
}