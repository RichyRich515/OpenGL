#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

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
	~cGameObject();

	Json::Value serializeJSONObject();

	// Move an object
	void translate(glm::vec3 velocity);

	// Transform the collision mesh by the world matrix
	void calculateCollisionMeshTransformed();

	std::string name;
	std::string meshName;
	cMesh* mesh;

	glm::mat4 matWorld;
	glm::mat4 inverseTransposeMatWorld;

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
	typedef std::pair<cMesh*, cMesh*> MeshPair;

	union uCollisionObjectInfo
	{
		AABBminmax* minmax; // For AABB
		float radius; // For sphere or capsule
		float height; // For capsule
		glm::vec3 plane; // For plane
		MeshPair* meshes; // For mesh, first is original mesh, second is transformed mesh
	} collisionObjectInfo;
};
