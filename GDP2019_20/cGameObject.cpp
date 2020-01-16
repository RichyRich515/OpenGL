#include "cGameObject.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <iostream>
#include <fstream>

cGameObject::cGameObject()
{
	this->name = "";
	this->meshName = "";
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


	if (pScript_init)
		delete pScript_init;
	if (pScript_update)
		delete pScript_update;

	if (this->command)
		delete this->command;
	// TODO: Capsule

	// TODO: manage children?
}

void cGameObject::instatiateBaseVariables(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	this->name = obj["name"].asString();
	this->type = obj["type"].asString();

	if (obj["script_init"])
	{
		this->script_init_name = obj["script_init"].asString();
		this->pScript_init = new cLuaBrain();
		this->pScript_init->loadScript(this->script_init_name, this);
	}
	if (obj["script_update"])
	{
		this->script_update_name = obj["script_update"].asString();
		this->pScript_update = new cLuaBrain();
		this->pScript_update->loadScript(this->script_update_name, this);
	}

	if (obj["texture"])
	{
		if (obj["texture"]["textures"])
		{
			unsigned tex_count = obj["texture"]["textures"].size();
			if (tex_count <= MAX_TEXTURES)
			{
				for (unsigned i = 0; i < tex_count; ++i)
				{
					this->textures[i].fileName = obj["texture"]["textures"][i]["name"].asString();
					this->textures[i].tiling = obj["texture"]["textures"][i]["tiling"].asFloat();
					this->textures[i].blend = obj["texture"]["textures"][i]["blend"].asFloat();
				}
			}
		}
		if (obj["texture"]["heightmap"])
		{
			this->heightmap.fileName = obj["texture"]["heightmap"]["name"].asString();
			this->heightmap.tiling = obj["texture"]["heightmap"]["tiling"].asFloat();
			this->heightmap.blend = obj["texture"]["heightmap"]["scale"].asFloat();
		}
		if (obj["texture"]["discardmap"])
		{
			this->discardmap.fileName = obj["texture"]["discardmap"]["name"].asString();
			this->discardmap.tiling = obj["texture"]["discardmap"]["tiling"].asFloat();
			this->discardmap.blend = obj["texture"]["discardmap"]["cutoff"].asFloat();
		}
	}

	this->meshName = obj["meshName"].asString();
	this->mesh = mapMeshes[this->meshName];
	// Load vec3s

	bool bpos = (bool)obj["position"];
	bool bvel = (bool)obj["velocity"];
	bool bacl = (bool)obj["acceleration"];
	for (unsigned j = 0; j < 3; ++j)
	{
		if (bpos)
			this->position[j] = obj["position"][j].asFloat();
		if (bvel)
			this->velocity[j] = obj["velocity"][j].asFloat();
		if (bacl)
			this->acceleration[j] = obj["acceleration"][j].asFloat();
	}

	bool bcol = (bool)obj["color"];
	bool bspc = (bool)obj["specular"];
	bool bori = (bool)obj["orientation"];
	// Load vec4s
	for (unsigned j = 0; j < 4; ++j)
	{
		if (bcol)
			this->color[j] = obj["color"][j].asFloat();
		if (bspc)
			this->specular[j] = obj["specular"][j].asFloat();
		if (bori)
			this->qOrientation[j] = obj["orientation"][j].asFloat();
	}
	if (!bori)
		this->qOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	if (obj["scale"])
		this->scale = obj["scale"].asFloat();
	else
		this->scale = 1.0f;

	if (obj["scale"])
		this->wireFrame = obj["wireFrame"].asBool();
	else
		this->wireFrame = false;

	if (obj["visible"])
		this->visible = obj["visible"].asBool();
	else
		this->visible = true;

	if (obj["inverseMass"])
		this->inverseMass = obj["inverseMass"].asFloat();
	else
		this->inverseMass = 0.0f;

	if (obj["bounciness"])
		this->bounciness = obj["bounciness"].asFloat();
	else
		this->bounciness = 0.0f;

	if (obj["lighting"])
		this->lighting = obj["lighting"].asBool();
	else
		this->lighting = true;

	if (obj["collisionObjectInfo"])
	{
		this->collisionShapeType = (eCollisionShapeType)obj["collisionObjectInfo"]["type"].asInt();
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
	}
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

	Json::Value text = Json::objectValue;
	bool textured = false;
	for (unsigned i = 0; i < MAX_TEXTURES; ++i)
	{
		if (this->textures[i].fileName.length())
		{
			Json::Value text_temp = Json::objectValue;
			text_temp["name"] = this->textures[i].fileName;
			text_temp["tiling"] = this->textures[i].tiling;
			text_temp["blend"] = this->textures[i].blend;
			text["textures"].append(text_temp);
			textured = true;
		}
	}
	if (this->heightmap.fileName.length())
	{
		text["heightmap"] = Json::objectValue;
		text["heightmap"]["name"] = this->heightmap.fileName;
		text["heightmap"]["tiling"] = this->heightmap.tiling;
		text["heightmap"]["scale"] = this->heightmap.blend;
		textured = true;
	}
	if (this->discardmap.fileName.length())
	{
		text["discardmap"] = Json::objectValue;
		text["discardmap"]["name"] = this->discardmap.fileName;
		text["discardmap"]["tiling"] = this->discardmap.tiling;
		text["discardmap"]["cutoff"] = this->discardmap.blend;
		textured = true;
	}

	if (textured)
		obj["texture"] = text;

	obj["meshName"] = this->meshName;

	bool bpos = position.x || position.y || position.z;
	bool bvel = velocity.x || velocity.y || velocity.z;
	bool bacl = acceleration.x || acceleration.y || acceleration.z;
	bool bcol = color.r || color.g || color.b || color.a;
	bool bspc = specular.r || specular.g || specular.b || specular.a;
	bool bori = qOrientation.x || qOrientation.y || qOrientation.z || qOrientation.w;

	// write vec3s
	for (unsigned j = 0; j < 3; ++j)
	{
		if (bpos)
			obj["position"][j] = this->position[j];
		if (bvel)
			obj["velocity"][j] = this->velocity[j];
		if (bacl)
			obj["acceleration"][j] = this->acceleration[j];
	}
	// write vec4s
	for (unsigned j = 0; j < 4; ++j)
	{
		if (bcol)
			obj["color"][j] = this->color[j];
		if (bspc)
			obj["specular"][j] = this->specular[j];
		if (bori)
			obj["orientation"][j] = this->qOrientation[j];
	}
	obj["scale"] = this->scale;

	if (this->wireFrame)
		obj["wireFrame"] = this->wireFrame;

	if (!this->visible)
		obj["visible"] = this->visible;

	if (this->inverseMass)
		obj["inverseMass"] = this->inverseMass;

	if (this->bounciness)
		obj["bounciness"] = this->bounciness;
	if (this->collisionShapeType != eCollisionShapeType::NONE)
	{
		Json::Value collisionObjectInfo = Json::objectValue;
		collisionObjectInfo["type"] = (int)this->collisionShapeType;
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

	}

	serializeUniqueVariables(obj);
	return obj;
}

void cGameObject::serializeUniqueVariables(Json::Value& obj)
{
	// None for base game object
}

void cGameObject::setPosition(glm::vec3 pos)
{
	this->position = pos;
}

void cGameObject::setPosition(float x, float y, float z)
{
	this->position = glm::vec3(x, y, z);
}

glm::vec3 cGameObject::getPosition()
{
	return this->position;
}

void cGameObject::setRotation(glm::vec3 rotation, bool deg)
{
	if (deg)
	{
		this->qOrientation = glm::quat(glm::radians(rotation));
		for (auto c : children)
		{
			c->qOrientation = glm::quat(glm::radians(rotation));
		}
	}
	else
	{
		this->qOrientation = glm::quat(rotation);
		for (auto c : children)
		{
			c->qOrientation = glm::quat(rotation);
		}
	}
}

void cGameObject::setOrientation(glm::quat q)
{
	this->qOrientation = q;
}

glm::quat cGameObject::getOrientation()
{
	return this->qOrientation;
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


void cGameObject::translate(glm::vec3 translation)
{
	this->position += translation;
	for (auto c : children)
	{
		c->position += translation;
	}
}

void cGameObject::rotate(glm::vec3 rotation, bool deg)
{
	if (deg)
	{
		this->qOrientation *= glm::quat(glm::radians(rotation));
		for (auto c : children)
		{
			c->qOrientation *= glm::quat(glm::radians(rotation));
		}
	}
	else
	{
		this->qOrientation *= glm::quat(rotation);
		for (auto c : children)
		{
			c->qOrientation *= glm::quat(rotation);
		}
	}
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
	if (pScript_init)
		pScript_init->Update(0.0f, 0.0f);
	if (command)
		command->init(0.0f, 0.0f);
}

void cGameObject::update(float dt, float tt)
{
	if (pScript_update)
		pScript_update->Update(dt, tt);

	if (command)
	{
		if (!command->isDone())
		{
			command->update(dt, tt);
		}
	}
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
