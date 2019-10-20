#pragma once

#include "iShip.hpp"
#include "cGameObject.hpp"

class cBasicShip : public iShip, public cGameObject
{
public:
	cBasicShip();
	cBasicShip(std::string name);
	cBasicShip(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual ~cBasicShip();
	// Inherited via cGameObject
	virtual void init();
	virtual void update(float dt);

	virtual sMessage message(sMessage const& msg);

	virtual void instatiateUniqueVariables(Json::Value& obj);
	virtual void serializeUniqueVariables(Json::Value& obj);

	// Inherited via iShip
	virtual void setEngine(iEngine* engine) override;
	virtual void setWeapon(iWeapon* weapon) override;

private:
	iEngine* _engine;
	iWeapon* _weapon;

	cGameObject* destination;
};