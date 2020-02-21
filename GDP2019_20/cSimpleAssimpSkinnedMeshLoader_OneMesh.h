#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <vector>
#include <array>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

// Used by the game object and the rendering
//#include "../cMesh.h"			// For loading into the VAO manager
//#include "cMeshObject.h"
#include "cMesh.hpp"

//#include "../sMeshDrawInfo.h"	// For the draw call

//Inspired by this page: http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html


class cSimpleAssimpSkinnedMesh
{
public:
	// Is STATIC const, so can be assigned a value here
	// (non static const have to be set in the cpp file)
	static const int MAX_BONES_PER_VERTEX = 4;
private:

	struct sVertexBoneData
	{
		//std::array<unsigned int, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> weights;

		void AddBoneData(unsigned int BoneID, float Weight);
	};

	struct sBoneInfo
	{
		std::string boneName;
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
		glm::mat4 ObjectBoneTransformation;
	};

public:
	// Mesh and VAO and VBO infor
	//unsigned int m_VBO_ID;				// Vertex buffer object 
	unsigned int m_numberOfVertices;
	//unsigned int m_indexBuf_ID;			// Index buffer referring to VBO
	unsigned int m_numberOfIndices;
	unsigned int m_numberOfTriangles;
	//bool m_bVBO_created;
	//// Remember: a VAO is a VBO+shaderProgram combination (i.e. you need 1 VAO ++PER++ VBO+shaderID)
	//// (this is only an issue if you are using mulitple shaders; if you are using a single "uber" shader, you're fine)
	//unsigned int m_VAO_ID;				// Vertex Array Object
	//unsigned int m_shaderIDMatchingVAO;	// The shader prod ID matching this VBO
	//bool m_bVAO_created;
//	// Actual vertex data
//	vert_XYZ_RGBA_N_STU_TanBi_4Bones* m_pVertexData;

public:
	cSimpleAssimpSkinnedMesh(void);
	virtual ~cSimpleAssimpSkinnedMesh(void);

	bool LoadMeshFromFile(const std::string& friendlyName, const std::string& filename);
	bool LoadMeshAnimation(const std::string& friendlyName, const std::string& filename, unsigned index = 0);

	// Added to assist with the forward kinematics (id and bone names)
	void GetListOfBoneIDandNames(std::vector<std::string>& vecBoneNames);

	std::string fileName;
	std::string friendlyName;

	const aiScene* pScene;
	Assimp::Importer* mImporter;

	struct sAnimationInfo
	{
		unsigned index;
		std::string friendlyName;
		std::string fileName;
		const aiScene* pAIScene;
	};


	std::map<std::string, sAnimationInfo> mapAnimationFriendlyNameTo_pScene;

	// Looks in the animation map and returns the total time
	float FindAnimationTotalTime(std::string animationName);

	// Returns NULL (0) if there is no mesh at that index
	cMesh* CreateMeshObjectFromCurrentModel(unsigned int meshIndex = 0);

	void BoneTransform(float TimeInSeconds, std::string animationName, std::vector<glm::mat4>& FinalTransformation, std::vector<glm::mat4>& Globals, std::vector<glm::mat4>& Offsets);

	unsigned int GetNums(void) const { return this->mNumBones; }

	float GetDuration(void);

	std::vector<sVertexBoneData> vecVertexBoneData;	//Bones;
	glm::mat4 mGlobalInverseTransformation;


	bool Initialize(void);

	void CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out);
	void CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);
	void CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);

	void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out);
	void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);
	void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName);
	
	void ReadNodeHeirarchy(float AnimationTime, const aiAnimation* pAnimation, const aiNode* pNode, const glm::mat4& parentTransformMatrix);

	void LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData>& Bones);

	void ShutErDown(void);

	std::map<std::string, unsigned int> m_mapBoneNameToBoneIndex;
	std::vector<sBoneInfo> mBoneInfo;
	unsigned int mNumBones;

};