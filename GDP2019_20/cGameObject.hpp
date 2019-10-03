#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <utility>

#include "cMesh.hpp"

enum eCollisionShapeTypes
{
	AABB,
	SPHERE,
	CAPSULE,
	PLANE,
	MESH,
	UNKNOWN
};

class cGameObject
{
public:

	cGameObject()
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

	cGameObject(std::string name)
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

	~cGameObject()
	{
		// Clean up
		if (collisionShapeType == AABB)
			delete collisionObjectInfo.minmax;
	}

	std::string name;
	std::string meshName;
	cMesh* mesh;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;
	glm::vec4 color;
	glm::vec4 specular; // A IS SHINYNESS, 1 to 10000
	bool wireFrame;
	bool visible;

	glm::vec3 velocity;
	glm::vec3 acceleration;
	float inverseMass; // Set 0 to ignore during update
	float bounciness; // Set 0 stop when hitting floor, // Set 1 to maintain 100 of velocity magnitude
	
	eCollisionShapeTypes collisionShapeType;

	typedef std::pair<glm::vec3, glm::vec3> AABBminmax;

	union uCollisionObjectInfo
	{
		AABBminmax* minmax; // For AABB
		float radius; // For sphere or capsule
		float height; // For capsule
		glm::vec3 plane; // For plane
		cMesh* mesh; // For mesh
	} collisionObjectInfo;
};
