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

void cSkinMeshComponent::preFrame(float dt, float tt)
{
}

void cSkinMeshComponent::update(float dt, float tt)
{
	anim_time += dt;

	if (anim_time > total_animation_time)
	{
		if (this->looping)
		{
			// nothing, loop
		}
		else
		{
			startNextAnimation();
		}
	}
}

void cSkinMeshComponent::render(float dt, float tt)
{
	auto pShader = cShaderManager::getCurrentShader();

	// todo: dont make new vectors every frame?
	std::vector<glm::mat4x4> vecFinalTransformation;
	std::vector<glm::mat4x4> vecOffsets;
	std::vector<glm::mat4x4> vecObjectBoneTransformation;
	
	this->skinmesh.BoneTransform(anim_time, this->current_animation, vecFinalTransformation, vecObjectBoneTransformation, vecOffsets);
	GLint numBonesUsed = (GLint)vecFinalTransformation.size();
	glUniformMatrix4fv(pShader->getUniformLocID("matBonesArray[0]"), numBonesUsed, GL_FALSE, glm::value_ptr(vecFinalTransformation[0]));
}

void cSkinMeshComponent::queueAnimation(std::string animationName, bool loop)
{
	if (this->animation_queue.size() < MAX_ANIMATION_QUEUE_SIZE)
	{
		this->animation_queue.push(std::make_pair(animationName, loop));
	}
}

void cSkinMeshComponent::forceNextAnimation()
{
	startNextAnimation();
}

void cSkinMeshComponent::forceAnimation(std::string animationName, bool loop, bool resetTime)
{
	this->current_animation = animationName;
	this->looping = loop;
	if (resetTime)
		this->anim_time = 0.0f;
	this->total_animation_time = this->skinmesh.FindAnimationTotalTime(animationName);
}

void cSkinMeshComponent::startNextAnimation()
{
	if (!this->animation_queue.empty())
	{
		this->current_animation = this->animation_queue.front().first;
		this->looping = this->animation_queue.front().second;
		this->total_animation_time = this->skinmesh.FindAnimationTotalTime(this->animation_queue.front().first);
		this->animation_queue.pop();
		this->anim_time = 0.0f;
	}
}

