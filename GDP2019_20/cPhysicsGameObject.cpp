#include "cPhysicsGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <iostream>
#include "GLCommon.h"
#include "cPhysicsManager.hpp"
#include "JsonHelper.hpp"
#include "cClothMeshComponent.hpp"
#include "cWorld.hpp"

cPhysicsGameObject::cPhysicsGameObject()
{
	this->name = "";
	this->type = "basic";
}

cPhysicsGameObject::cPhysicsGameObject(std::string name)
{
	this->name = name;
	this->type = "basic";
}

cPhysicsGameObject::cPhysicsGameObject(Json::Value& obj)
{
	this->instatiateBaseVariables(obj);
}

cPhysicsGameObject::cPhysicsGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->instatiateBaseVariables(obj);
}

cPhysicsGameObject::~cPhysicsGameObject()
{
	cPhysicsManager::getWorld()->RemoveComponent(this->physics);
	if (this->physics)
		delete this->physics;
	if (this->mesh)
		delete this->mesh;
}

void cPhysicsGameObject::instatiateBaseVariables(const Json::Value& obj)
{
	this->name = obj["name"] ? obj["name"].asString() : ""; 
	this->type = obj["type"] ? obj["type"].asString() : "";

	if (obj["graphics"])
		this->graphics.instatiateBaseVariables(obj["graphics"]);


	if (obj["physics"])
	{
		std::string type = obj["physics"]["type"] ? obj["physics"]["type"].asString() : "";
		if (type != "cloth" && obj["mesh"])
		{
			this->mesh = new cMeshComponent();
			this->mesh->instatiateBaseVariables(obj["mesh"]);
		}
		else
		{
			this->mesh = new cClothMeshComponent();
			this->mesh->instatiateBaseVariables(obj["mesh"]);
		}

		if (type == "ball")
		{
			nPhysics::sBallDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Radius = obj["physics"]["radius"] ? obj["physics"]["radius"].asFloat() : 1.0f;
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateBall(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "box")
		{
			nPhysics::sBoxDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Extents = obj["physics"]["extents"] ? Json::toVec3(obj["physics"]["extents"]) : glm::vec3(1.0f);
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateBox(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "sliding_box")
		{
			nPhysics::sSlidingBoxDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Extents = obj["physics"]["extents"] ? Json::toVec3(obj["physics"]["extents"]) : glm::vec3(1.0f);
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateSlidingBox(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "cylinder")
		{
			nPhysics::sCylinderDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Extents = obj["physics"]["extents"] ? Json::toVec3(obj["physics"]["extents"]) : glm::vec3(1.0f);
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateCylinder(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "free_swinging_cylinder")
		{
			nPhysics::sFreeSwingingCylinderDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Extents = obj["physics"]["extents"] ? Json::toVec3(obj["physics"]["extents"]) : glm::vec3(1.0f);
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateFreeSwingingCylinder(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "cone")
		{
			nPhysics::sConeDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Radius = obj["physics"]["radius"] ? obj["physics"]["radius"].asFloat() : 1.0f;
			def.Height = obj["physics"]["height"] ? obj["physics"]["height"].asFloat() : 1.0f;
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateCone(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "swinging_cone")
		{
			nPhysics::sSwingingConeDef def;
			def.Elasticity = obj["physics"]["elasticity"] ? obj["physics"]["elasticity"].asFloat() : 0.0f;
			def.Radius = obj["physics"]["radius"] ? obj["physics"]["radius"].asFloat() : 1.0f;
			def.Height = obj["physics"]["height"] ? obj["physics"]["height"].asFloat() : 1.0f;
			def.Mass = obj["physics"]["mass"] ? obj["physics"]["mass"].asFloat() : 0.0f;
			def.Position = obj["physics"]["position"] ? Json::toVec3(obj["physics"]["position"]) : glm::vec3(0.0f);

			this->physics = cPhysicsManager::getFactory()->CreateSwingingCone(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "plane")
		{
			glm::vec3 point = obj["physics"]["point"] ? Json::toVec3(obj["physics"]["point"]) : glm::vec3(0.0f);

			nPhysics::sPlaneDef def;
			def.Normal = obj["physics"]["normal"] ? Json::toVec3(obj["physics"]["normal"]) : glm::vec3(0.0f);
			def.Constant = glm::dot(def.Normal, point);

			this->physics = cPhysicsManager::getFactory()->CreatePlane(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);
		}
		else if (type == "cloth")
		{
			nPhysics::sClothDef def;

			def.CornerA = obj["physics"]["cornerA"] ? Json::toVec3(obj["physics"]["cornerA"]) : glm::vec3(0.0f);
			def.CornerB = obj["physics"]["cornerB"] ? Json::toVec3(obj["physics"]["cornerB"]) : glm::vec3(0.0f);
			def.DownDirection = obj["physics"]["downDirection"] ? Json::toVec3(obj["physics"]["downDirection"]) : glm::vec3(0.0f);
			def.NodeMass = obj["physics"]["nodeMass"] ? obj["physics"]["nodeMass"].asFloat() : 0.0f;
			def.NumNodesAcross = obj["physics"]["numNodesAcross"] ? obj["physics"]["numNodesAcross"].asFloat() : 0.0f;
			def.NumNodesDown = obj["physics"]["numNodesDown"] ? obj["physics"]["numNodesDown"].asFloat() : 0.0f;
			def.SpringConstant = obj["physics"]["springConstant"] ? obj["physics"]["springConstant"].asFloat() : 0.0f;

			this->physics = cPhysicsManager::getFactory()->CreateCloth(def);
			cPhysicsManager::getWorld()->AddComponent(this->physics);

			auto cmcomp = new cClothMeshComponent();
			cmcomp->cloth = (nPhysics::iClothComponent*)this->physics;
			cmcomp->scale = 1.0f;
			this->mesh = cmcomp;
		}
	}
}

void cPhysicsGameObject::instatiateUniqueVariables(const Json::Value& obj)
{
	// None for base game object
}

void cPhysicsGameObject::serializeJSONObject(Json::Value& obj)
{
	obj = Json::objectValue;
	obj["name"] = this->name;
	obj["type"] = this->type;


	serializeUniqueVariables(obj);
}

void cPhysicsGameObject::serializeUniqueVariables(Json::Value& obj)
{
	// None for base game object
}


void cPhysicsGameObject::init()
{
}

eComponentType cPhysicsGameObject::getType()
{
	return eComponentType::GameObject;
}

glm::vec3 cPhysicsGameObject::getPosition()
{
	if (this->physics)
	{
		glm::mat4 transform;
		this->physics->GetTransform(transform);
		return transform[3];
	}
	else
	{
		return glm::vec3(0.0f);
	}
}

void cPhysicsGameObject::preFrame(float dt, float tt)
{
}

void cPhysicsGameObject::render(float dt, float tt)
{
	if (!this->graphics.visible)
		return;

	glm::mat4 m(1.0f);
	this->physics->GetTransform(m);
	m *= glm::scale(glm::mat4(1.0), glm::vec3(this->mesh->useLOD ? this->mesh->lodScale : this->mesh->scale));
	cShaderManager::cShaderProgram* pShader = cShaderManager::getCurrentShader();
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render(dt, tt);
	this->mesh->render(dt, tt);
}

void cPhysicsGameObject::update(float dt, float tt)
{
	this->graphics.update(dt, tt);
	this->mesh->update(dt, tt);
	if (!this->mesh->lodMeshName.empty())
	{
		this->mesh->useLOD = glm::distance(cWorld::camera->position, this->getPosition()) > this->mesh->lodDistance;
	}
}

sMessage cPhysicsGameObject::message(sMessage const& msg)
{
	return sMessage();
}
