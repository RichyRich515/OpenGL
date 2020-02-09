#pragma once

#include <string>
#include <map>

#include "iGameObject.hpp"

#include "iGameObjectFactory.hpp"

class cGameObjectFactory : public iGameObjectFactory
{
public:
	// Inherited via iGameObjectFactory
	virtual iGameObject* createFromJSON(Json::Value& obj, std::map<std::string, cMesh*> & mapMeshes) override;
	virtual iGameObject* createFromType(std::string type, std::map<std::string, cMesh*>& mapMeshes) override;
	virtual std::string getTypeString() override;
};