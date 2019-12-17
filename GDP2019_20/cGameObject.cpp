#include "cGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <iostream>

cGameObject::cGameObject()
{
	this->name = "";
	this->meshName = "";
	this->textureName = "";
	this->type = "basic";
	this->mesh = NULL;
	this->position = glm::vec3(0);
	this->qOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	this->scale = 1.0f;
	this->color = glm::vec4(1.0f);
	this->specular = glm::vec4(1.0f);
	this->wireFrame = false;
	this->visible = true;
	this->lighting = true;
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->inverseMass = 0.0f;
}

cGameObject::cGameObject(std::string name)
{
	this->name = name;
	this->meshName = "";
	this->textureName = "";
	this->type = "basic";
	this->mesh = NULL;
	this->position = glm::vec3(0);
	this->qOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	this->scale = 1.0f;
	this->color = glm::vec4(1.0f);
	this->specular = glm::vec4(1.0f);
	this->wireFrame = false;
	this->visible = true;
	this->lighting = true;
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->inverseMass = 0.0f;
}

cGameObject::cGameObject(Json::Value& obj, std::map<std::string, cMesh*> & mapMeshes)
{
	this->instatiateBaseVariables(obj, mapMeshes);
}

cGameObject::~cGameObject()
{
	// Clean up
	if (collisionShapeType == eCollisionShapeType::AABB)
		delete collisionObjectInfo.minmax;
	else if (collisionShapeType == eCollisionShapeType::MESH || collisionShapeType == eCollisionShapeType::STATIC_MESH_AABBS)
	{
		delete this->collisionObjectInfo.meshes->second; // Main mesh is cleaned by mesh manager
		delete this->collisionObjectInfo.meshes;
	}
	else if (collisionShapeType == eCollisionShapeType::POINT_LIST)
	{
		delete this->collisionObjectInfo.points;
	}
	// TODO: Capsule
}

void cGameObject::instatiateBaseVariables(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->name = obj["name"].asString();
	this->type = obj["type"].asString();
	this->textureName = obj["textureName"].asString();
	this->meshName = obj["meshName"].asString();
	this->mesh = mapMeshes[this->meshName];
	// Load vec3s
	for (unsigned j = 0; j < 3; ++j)
	{
		this->position[j] = obj["position"][j].asFloat();
		this->velocity[j] = obj["velocity"][j].asFloat();
		this->acceleration[j] = obj["acceleration"][j].asFloat();
	}
	// Load vec4s
	for (unsigned j = 0; j < 4; ++j)
	{
		this->color[j] = obj["color"][j].asFloat();
		this->specular[j] = obj["specular"][j].asFloat();
		this->qOrientation[j] = obj["rotation"][j].asFloat();
	}
	this->scale = obj["scale"].asFloat();
	this->wireFrame = obj["wireFrame"].asBool();
	this->visible = obj["visible"].asBool();
	this->inverseMass = obj["inverseMass"].asFloat();
	this->bounciness = obj["bounciness"].asFloat();
	this->collisionShapeType = (eCollisionShapeType)obj["collisionShapeType"].asInt();
	Json::Value collisionObjectInfo = obj["collisionObjectInfo"];
	switch (this->collisionShapeType)
	{
	case eCollisionShapeType::AABB:
	{
		this->collisionObjectInfo.minmax = new cGameObject::AABBminmax();
		// load vec3s
		for (unsigned j = 0; j < 3; ++j)
		{
			this->collisionObjectInfo.minmax->first[j] = collisionObjectInfo["min"][j].asFloat();
			this->collisionObjectInfo.minmax->second[j] = collisionObjectInfo["max"][j].asFloat();
		}
		break;
	}
	case eCollisionShapeType::OBB:
		break;
	case eCollisionShapeType::SPHERE:
		this->collisionObjectInfo.radius = collisionObjectInfo["radius"].asFloat();
		break;
	case eCollisionShapeType::CAPSULE:
		break;
	case eCollisionShapeType::PLANE:
		break;
	case eCollisionShapeType::MESH:
	{
		this->collisionObjectInfo.meshes = new MeshPair();
		this->collisionObjectInfo.meshes->first = mapMeshes[collisionObjectInfo["mesh"].asString()];
		cMesh* collMesh = new cMesh();
		*collMesh = *this->collisionObjectInfo.meshes->first;
		this->collisionObjectInfo.meshes->second = collMesh;
		break;
	}
	case eCollisionShapeType::POINT_LIST:
	{
		unsigned pointsCount = collisionObjectInfo["points"].size();
		//glm::vec3* plist = new glm::vec3[pointsCount];
		this->collisionObjectInfo.points = new PointsList;
		this->collisionObjectInfo.points->reserve(pointsCount);
		for (unsigned i = 0; i < pointsCount; ++i)
		{
			glm::vec3 p;
			for (unsigned j = 0; j < 3; ++j)
			{
				p[j] = collisionObjectInfo["points"][i][j].asFloat();
			}
			this->collisionObjectInfo.points->push_back(p);
		}
		break;
	}
	case eCollisionShapeType::STATIC_MESH_AABBS:
	{
		this->collisionObjectInfo.meshes = new MeshPair();
		this->collisionObjectInfo.meshes->first = mapMeshes[collisionObjectInfo["mesh"].asString()];
		cMesh* collMesh = new cMesh();
		*collMesh = *this->collisionObjectInfo.meshes->first;
		this->collisionObjectInfo.meshes->second = collMesh;
		break;
	}
	default:
		break;
	}

	// TODO: load lighting from file
	this->lighting = true;
}

void cGameObject::instatiateUniqueVariables(Json::Value& obj)
{
	// None for base game object
}

Json::Value cGameObject::serializeJSONObject()
{
	Json::Value obj = Json::objectValue;
	obj["name"] = this->name;
	obj["type"] = this->type;
	obj["textureName"] = this->textureName;
	obj["meshName"] = this->meshName;
	// write vec3s
	for (unsigned j = 0; j < 3; ++j)
	{
		obj["position"][j] = this->position[j];
		obj["velocity"][j] = this->velocity[j];
		obj["acceleration"][j] = this->acceleration[j];
	}
	// write vec4s
	for (unsigned j = 0; j < 4; ++j)
	{
		obj["color"][j] = this->color[j];
		obj["specular"][j] = this->specular[j];
		obj["rotation"][j] = this->qOrientation[j];
	}
	obj["scale"] = this->scale;
	obj["wireFrame"] = this->wireFrame;
	obj["visible"] = this->visible;
	obj["inverseMass"] = this->inverseMass;
	obj["bounciness"] = this->bounciness;
	obj["collisionShapeType"] = (int)this->collisionShapeType;
	Json::Value collisionObjectInfo = Json::objectValue;
	switch (this->collisionShapeType)
	{
	case eCollisionShapeType::AABB:
	{
		// write vec3s
		for (unsigned j = 0; j < 3; ++j)
		{
			collisionObjectInfo["min"][j] = this->collisionObjectInfo.minmax->first[j];
			collisionObjectInfo["max"][j] = this->collisionObjectInfo.minmax->second[j];
		}
		break;
	}
	case eCollisionShapeType::OBB:
		break;
	case eCollisionShapeType::SPHERE:
		collisionObjectInfo["radius"] = this->collisionObjectInfo.radius;
		break;
	case eCollisionShapeType::CAPSULE:
		break;
	case eCollisionShapeType::PLANE:
		break;
	case eCollisionShapeType::MESH:
		collisionObjectInfo["mesh"] = this->meshName; // TODO: copy collision mesh name instead
		break;
	case eCollisionShapeType::POINT_LIST:
		// point list
		for (unsigned i = 0; i < this->collisionObjectInfo.points->size(); ++i)
		{
			for (unsigned j = 0; j < 3; ++j)
			{
				collisionObjectInfo["points"] = (*this->collisionObjectInfo.points)[i][j];
			}
		}
		break;
	case eCollisionShapeType::STATIC_MESH_AABBS:
		collisionObjectInfo["mesh"] = this->meshName; // TODO: copy collision mesh name instead
		break;
	default:
		break;
	}
	obj["collisionObjectInfo"] = collisionObjectInfo;

	serializeUniqueVariables(obj);
	return obj;
}

void cGameObject::serializeUniqueVariables(Json::Value& obj)
{
	// nothing
}

void cGameObject::calculateCollisionMeshTransformed()
{
	for (std::vector<sPlyVertex>::iterator origVertItr = this->collisionObjectInfo.meshes->first->vecVertices.begin(),
										   collVertItr = this->collisionObjectInfo.meshes->second->vecVertices.begin();
		origVertItr != this->collisionObjectInfo.meshes->first->vecVertices.end();
		// they are copies of each other so same size
		++origVertItr, ++collVertItr)
	{
		glm::vec4 verts = glm::vec4(origVertItr->x, origVertItr->y, origVertItr->z, 1.0f);
		glm::vec4 norms = glm::vec4(origVertItr->nx, origVertItr->ny, origVertItr->nz, 1.0f);
		verts = this->matWorld * verts;
		norms = this->inverseTransposeMatWorld * norms;
		collVertItr->x = verts.x;
		collVertItr->y = verts.y;
		collVertItr->z = verts.z;
		collVertItr->nx = norms.x;
		collVertItr->ny = norms.y;
		collVertItr->nz = norms.z;
	}
}


void cGameObject::translate(glm::vec3 velocity)
{
	this->position += velocity;
}

void cGameObject::rotate(glm::vec3 rotation, bool deg)
{
	this->qOrientation *= glm::quat(rotation);
}

void cGameObject::updateMatricis()
{
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), this->position);
	glm::mat4 rotation = glm::mat4(this->qOrientation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale, this->scale, this->scale));

	this->matWorld = glm::mat4(1.0f);
	this->matWorld *= translation;
	this->matWorld *= rotation;
	this->matWorld *= scale;
	this->inverseTransposeMatWorld = glm::inverse(glm::transpose(this->matWorld));
	if (this->collisionShapeType == eCollisionShapeType::MESH) // TODO: mesh aabb???
		this->calculateCollisionMeshTransformed();
}

void cGameObject::init()
{

}

void cGameObject::update(float dt)
{
	
}

void cGameObject::physicsUpdate(float dt)
{
	this->velocity.x += (this->acceleration.x /* + gravity.x */) * dt;
	this->velocity.y += (this->acceleration.y /* + gravity.y */) * dt;
	this->velocity.z += (this->acceleration.z /* + gravity.z */) * dt;
	this->position.x += this->velocity.x * dt;
	this->position.y += this->velocity.y * dt;
	this->position.z += this->velocity.z * dt;
}

sMessage cGameObject::message(sMessage const& msg)
{

	return sMessage();
}
