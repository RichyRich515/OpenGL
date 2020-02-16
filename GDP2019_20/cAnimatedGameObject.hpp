#pragma once

#include "cGameObject.hpp"
#include "cSkinMeshComponent.hpp"

class cAnimatedGameObject : public iGameObject
{
public:
	cAnimatedGameObject();
	cAnimatedGameObject(std::string name);
	cAnimatedGameObject(Json::Value& obj);
	cAnimatedGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);

	virtual eComponentType getType() override;

	virtual sMessage message(sMessage const& msg) override;

	virtual void init() override;
	virtual void preFrame() override;
	virtual void update(float dt, float tt) override;
	virtual void render() override;
	virtual glm::vec3 getPosition() override;

	cSkinMeshComponent skinmesh;
	cMeshComponent mesh;
	cGraphicsComponent graphics;
	cTransformComponent transform;

};