#pragma once

#include "iGameObjectFactory.hpp"

class cSpaceGameFactory : public iGameObjectFactory
{
	// Inherited via iGameObjectFactory
	virtual cGameObject* createFromJSON(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes) override;
	virtual cGameObject* createFromType(std::string type) override;
	virtual std::string getTypeString() override;
};