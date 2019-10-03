#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <map>
#include <utility>

#include <json/json.h>

#include "cMesh.hpp"

enum eCollisionShapeTypes
{
	NONE = 0,
	AABB,
	OBB,
	SPHERE,
	CAPSULE,
	PLANE,
	MESH,
	UNKNOWN
};

class cGameObject
{
public:

	cGameObject();

	cGameObject(std::string name);

	cGameObject(Json::Value obj, std::map<std::string, cMesh*> & mapMeshes);

	~cGameObject()
	{
		// Clean up
		if (collisionShapeType == AABB)
			delete collisionObjectInfo.minmax;
		// TODO: Capsule
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
