#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

#include <json/json.h>

#include "iMessageable.hpp"
//#include "cTexture.hpp"
//#include "Lua/cLuaBrain.hpp"
//#include "cCommand.hpp"
//#include "cMesh.hpp"
//#include <glm/vec3.hpp>
//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp>
//#include <glm/gtx/quaternion.hpp>

#include <iPhysicsComponent.h>
#include "iComponent.hpp"
#include "cGraphicsComponent.hpp"
#include "cMeshComponent.hpp"

//enum class eCollisionShapeType
//{
//	NONE = 0,
//	AABB,
//	OBB,
//	SPHERE,
//	CAPSULE,
//	PLANE,
//	MESH,
//	POINT_LIST,
//	STATIC_MESH_AABBS,
//	UNKNOWN
//};

class cGameObject : public iMessageable
{
public:
	unsigned id;

	std::string name;
	std::string type;

	cGameObject();
	cGameObject(std::string name);
	cGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual ~cGameObject();

	virtual void init();
	virtual void update(float dt, float tt);
	//void physicsUpdate(float dt);
	virtual sMessage message(sMessage const& msg);

	// Constructor will call this
	virtual void instatiateBaseVariables(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes);
	virtual void instatiateUniqueVariables(Json::Value& obj);

	Json::Value serializeJSONObject();
	virtual void serializeUniqueVariables(Json::Value& obj);

	cMeshComponent mesh;
	cGraphicsComponent graphics;
	nPhysics::iPhysicsComponent* physics;
	
	//std::string script_init_name;
	//std::string script_update_name;
	//cLuaBrain* pScript_init;
	//cLuaBrain* pScript_update;
	//iCommand* command;
	//cMesh* mesh;
//private:
//	cGameObject* parent;
//	std::vector<cGameObject*> children;
//
//public:
//	glm::vec3 position;
//	glm::quat qOrientation;
	//glm::vec3 velocity;
	//glm::vec3 acceleration;
	//float scale;
	//float inverseMass; // Set 0.0f to ignore during physics
	//float bounciness; // Set 0.0f stop when hitting, Set to 1.0f maintain 100% of velocity
	//
	//// Transform the collision mesh by the world matrix
	//void calculateCollisionMeshTransformed();
	//
	//eCollisionShapeType collisionShapeType;
	//
	//typedef std::pair<glm::vec3, glm::vec3> AABBminmax;
	//typedef std::pair<cMesh*, cMesh*> MeshPair;
	//typedef std::vector<glm::vec3> PointsList;
	//
	//union uCollisionObjectInfo
	//{
	//	AABBminmax* minmax; // For AABB
	//	float radius; // For sphere
	//	float height; // TODO: For capsule
	//	glm::vec3 plane; // TODO: For plane
	//	MeshPair* meshes; // For mesh, first is original mesh, second is transformed mesh
	//	PointsList* points;
	//} collisionObjectInfo;
};
