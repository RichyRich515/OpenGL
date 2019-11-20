#pragma once

#include <string>
#include <map>

#include "cGameObject.hpp"

#include "iGameObjectFactory.hpp"

class cGameObjectFactory : public iGameObjectFactory
{
public:
	// Inherited via iGameObjectFactory
	virtual cGameObject* createFromJSON(Json::Value& obj, std::map<std::string, cMesh*> & mapMeshes) override;
	virtual cGameObject* createFromType(std::string type, std::map<std::string, cMesh*>& mapMeshes) override;
	virtual std::string getTypeString() override;
};