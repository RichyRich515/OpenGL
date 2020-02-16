#pragma once

#include "iComponent.hpp"
#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

class cSkinMeshComponent : public iComponent
{
public:
	// Inherited via iComponent
	virtual void init() override;
	virtual eComponentType getType() override;
	virtual void preFrame() override;
	virtual void update(float dt, float tt) override;
	virtual void render() override;

	cSimpleAssimpSkinnedMesh skinmesh;

	float dt;
};