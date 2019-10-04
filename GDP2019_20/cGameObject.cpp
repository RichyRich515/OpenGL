#include "cGameObject.hpp"

cGameObject::cGameObject()
{
	this->name = "";
	this->meshName = "";
	this->mesh = NULL;
	this->position = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->scale = 1.0f;
	this->color = glm::vec4(1.0f);
	this->specular = glm::vec4(1.0f);
	this->wireFrame = false;
	this->visible = true;
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->inverseMass = 0.0f;
}

cGameObject::cGameObject(std::string name)
{
	this->name = name;
	this->meshName = "";
	this->mesh = NULL;
	this->position = glm::vec3(0);
	this->rotation = glm::vec3(0);
	this->scale = 1.0f;
	this->color = glm::vec4(1.0f);
	this->specular = glm::vec4(1.0f);
	this->wireFrame = false;
	this->visible = true;
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->inverseMass = 0.0f;
}

cGameObject::cGameObject(Json::Value obj, std::map<std::string, cMesh*> & mapMeshes)
{
	this->meshName = obj["meshName"].asString();
	this->mesh = mapMeshes[this->meshName];
	// Load vec3s
	for (unsigned j = 0; j < 3; ++j)
	{
		this->position[j] = obj["position"][j].asFloat();
		this->rotation[j] = obj["rotation"][j].asFloat();
		this->velocity[j] = obj["velocity"][j].asFloat();
		this->acceleration[j] = obj["acceleration"][j].asFloat();
	}
	// Load vec4s
	for (unsigned j = 0; j < 4; ++j)
	{
		this->color[j] = obj["color"][j].asFloat();
		this->specular[j] = obj["specular"][j].asFloat();
	}
	this->scale = obj["scale"].asFloat();
	this->wireFrame = obj["wireFrame"].asBool();
	this->visible = obj["visible"].asBool();
	this->inverseMass = obj["inverseMass"].asFloat();
	this->bounciness = obj["bounciness"].asFloat();
	this->collisionShapeType = (eCollisionShapeTypes)obj["collisionShapeType"].asInt();
	Json::Value collisionObjectInfo = obj["collisionObjectInfo"];
	switch (this->collisionShapeType)
	{
	case AABB:
	{
		this->collisionObjectInfo.minmax = new cGameObject::AABBminmax();
		// load vec3s
		for (unsigned j = 0; j < 3; ++j)
		{
			this->collisionObjectInfo.minmax->first[j] = collisionObjectInfo["min"][j].asFloat();
			this->collisionObjectInfo.minmax->second[j] = collisionObjectInfo["max"][j].asFloat();
		}
		this->collisionObjectInfo.minmax->first += this->position;
		this->collisionObjectInfo.minmax->second += this->position;
		break;
	}
	case OBB:
		break;
	case SPHERE:
		this->collisionObjectInfo.radius = collisionObjectInfo["radius"].asFloat();
		break;
	case CAPSULE:
		break;
	case PLANE:
		break;
	case MESH:
		this->collisionObjectInfo.mesh = mapMeshes[collisionObjectInfo["mesh"].asString()];
		break;
	default:
		break;
	}
}

cGameObject::~cGameObject()
{
	// Clean up
	if (collisionShapeType == AABB)
		delete collisionObjectInfo.minmax;
	// TODO: Capsule
}

Json::Value cGameObject::serializeJSONObject()
{
	Json::Value obj = Json::objectValue;
	obj["name"] = this->name;
	obj["meshName"] = this->meshName;
	// write vec3s
	for (unsigned j = 0; j < 3; ++j)
	{
		obj["position"][j] = this->position[j];
		obj["rotation"][j] = this->rotation[j];
		obj["velocity"][j] = this->velocity[j];
		obj["acceleration"][j] = this->acceleration[j];
	}
	// write vec4s
	for (unsigned j = 0; j < 4; ++j)
	{
		obj["color"][j] = this->color[j];
		obj["specular"][j] = this->specular[j];
	}
	obj["scale"] = this->scale;
	obj["wireFrame"] = this->wireFrame;
	obj["visible"] = this->visible;
	obj["inverseMass"] = this->inverseMass;
	obj["bounciness"] = this->bounciness;
	obj["collisionShapeType"] = (int)this->collisionShapeType ;
	Json::Value collisionObjectInfo = Json::objectValue;
	switch (this->collisionShapeType)
	{
	case AABB:
	{
		// write vec3s
		for (unsigned j = 0; j < 3; ++j)
		{
			collisionObjectInfo["min"][j] = this->collisionObjectInfo.minmax->first[j];
			collisionObjectInfo["max"][j] = this->collisionObjectInfo.minmax->second[j];
		}
		break;
	}
	case OBB:
		break;
	case SPHERE:
		collisionObjectInfo["radius"] = this->collisionObjectInfo.radius;
		break;
	case CAPSULE:
		break;
	case PLANE:
		break;
	case MESH:
		collisionObjectInfo["mesh"] = this->meshName;
		break;
	default:
		break;
	}
	obj["collisionObjectInfo"] = collisionObjectInfo;
	return obj;
}
