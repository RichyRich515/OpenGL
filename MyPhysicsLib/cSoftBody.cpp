#include "cSoftBody.h"
#include <numeric>

namespace phys
{
	cSoftBody::cSpring::cSpring(cNode* nodeA, cNode* nodeB, float springConstant) :
		NodeA(nodeA),
		NodeB(nodeB),
		RestingLength(glm::distance(nodeA->Position, nodeB->Position)),
		SpringConstant(springConstant),
		SpringForceAtoB(0.0f)
	{
	}

	void cSoftBody::cSpring::UpdateSpringForce()
	{
		// HOOKS LAW
		glm::vec3 sep = NodeB->Position - NodeA->Position;
		float dist = glm::length(sep);
		float x = dist - RestingLength;
		SpringForceAtoB = -SpringConstant * glm::normalize(sep) * x;
	}

	void cSoftBody::cSpring::ApplyForceToNodes()
	{
		if (!NodeB->IsFixed())
		{
			NodeB->Acceleration += SpringForceAtoB / NodeB->Mass;
		}
		if (!NodeA->IsFixed())
		{
			NodeA->Acceleration -= SpringForceAtoB / NodeA->Mass;
		}
	}

	cSoftBody::cNode* cSoftBody::cSpring::GetOther(cNode* node)
	{
		return node == NodeA ? NodeB : NodeA;
	}

	cSoftBody::cSoftBody(const sSoftBodyDef& def) :
		cCollisionBody(eBodyType::soft)
	{
		size_t numNodes = def.Nodes.size();
		mNodes.resize(numNodes);
		for (size_t i = 0; i < numNodes; ++i)
		{
			mNodes[i] = new cNode(def.Nodes[i]);
		}
		size_t numSprings = def.Springs.size();
		mSprings.resize(numSprings);
		for (size_t i = 0; i < numSprings; ++i)
		{
			mSprings[i] = new cSpring(mNodes[def.Springs[i].first], mNodes[def.Springs[i].second], def.SpringConstant);
			mNodes[def.Springs[i].first]->Springs.push_back(mSprings[i]);
			mNodes[def.Springs[i].second]->Springs.push_back(mSprings[i]);
		}

		for (size_t i = 0; i < numNodes; ++i)
		{
			mNodes[i]->CalculateRadius();
		}
	}

	cSoftBody::~cSoftBody()
	{
		size_t numNodes = mNodes.size();
		for (size_t i = 0; i < numNodes; ++i)
		{
			delete mNodes[i];
		}
		mNodes.clear();
		size_t numSprings = mSprings.size();
		for (size_t i = 0; i < numSprings; ++i)
		{
			delete mSprings[i];
		}
		mSprings.clear();
	}

	size_t cSoftBody::NumNodes()
	{
		return this->mNodes.size();
	}

	bool cSoftBody::GetNodeRadius(size_t index, float& radiusOut)
	{
		if (index >= mNodes.size()) return false;
		radiusOut = mNodes[index]->Radius;
		return true;
	}

	bool cSoftBody::GetNodePosition(size_t index, glm::vec3& positionOut)
	{
		if (index >= mNodes.size()) return false;
		positionOut = mNodes[index]->Position;
		return true;
	}

	void cSoftBody::ApplyForce(glm::vec3 force)
	{
		size_t numNodes = mNodes.size();
		for (size_t i = 0; i < numNodes; ++i)
		{
			this->mNodes[i]->ApplyForce(force);
		}
	}

	void cSoftBody::ClearAccelerations()
	{
		for (size_t i = 0; i < mNodes.size(); ++i)
		{
			mNodes[i]->Acceleration.x = 0;
			mNodes[i]->Acceleration.y = 0;
			mNodes[i]->Acceleration.z = 0;
		}
	}

	cSoftBody::cNode::cNode(const sSoftBodyNodeDef& nodeDef) :
		Position(nodeDef.Position),
		PreviousPosition(nodeDef.Position),
		Mass(nodeDef.Mass),
		InvMass(1.0f / nodeDef.Mass),
		Radius(1.0f),
		Velocity(0.0f),
		PreviousVelocity(0.0f),
		Acceleration(0.0f)
	{
	}

	void cSoftBody::cNode::CalculateRadius()
	{
		float smallestDistance = std::numeric_limits<float>::max();
		size_t numNeighbors = this->Springs.size();
		for (size_t i = 0; i < numNeighbors; ++i)
		{
			// is this the smallest distance?
			float dist = glm::distance(Springs[i]->GetOther(this)->Position, Position);
			if (dist < smallestDistance)
				smallestDistance = dist;
		}

		Radius = smallestDistance * 0.49f;
	}

	bool cSoftBody::cNode::IsNeighbour(cNode* other)
	{
		size_t numSprings = Springs.size();
		for (size_t i = 0; i < numSprings; ++i)
		{
			if (Springs[i]->GetOther(this) == other)
				return true;
		}
	}

	void cSoftBody::cNode::ApplyForce(glm::vec3 force)
	{
		this->Acceleration += force * this->InvMass;
	}
}