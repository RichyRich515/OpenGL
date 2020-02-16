#include "cSkinMeshComponent.hpp"
#include "cShaderManager.hpp"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

void cSkinMeshComponent::init()
{
}

eComponentType cSkinMeshComponent::getType()
{
	return eComponentType();
}

void cSkinMeshComponent::preFrame()
{
}

void cSkinMeshComponent::update(float dt, float tt)
{
	this->dt = dt;
}

void cSkinMeshComponent::render()
{
	auto pShader = cShaderManager::getCurrentShader();

	const int NUMBEROFBONES = 100;

	std::vector<glm::mat4x4> vecFinalTransformation;
	std::vector<glm::mat4x4> vecOffsets;
	std::vector<glm::mat4x4> vecObjectBoneTransformation;
	
	// TODO: frame time, animation name
	static float t = 0.0f;
	this->skinmesh.BoneTransform(t, "Walk", vecFinalTransformation, vecObjectBoneTransformation, vecOffsets);
	t += dt;
	GLint numBonesUsed = (GLint)vecFinalTransformation.size();
	glUniformMatrix4fv(pShader->getUniformLocID("matBonesArray[0]"), numBonesUsed, GL_FALSE, glm::value_ptr(vecFinalTransformation[0]));
}
