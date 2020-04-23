#include "cMeshComponent.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "cShaderManager.hpp"
#include "cVAOManager.hpp"

void cMeshComponent::init()
{
}

void cMeshComponent::update(float dt, float tt)
{
}

eComponentType cMeshComponent::getType()
{
	return eComponentType();
}

//void cMeshComponent::updateMatrices()
//{
//	this->matWorld = glm::mat4(1.0f);
//	this->matWorld *= glm::translate(glm::mat4(1.0f), this->position);
//	this->matWorld *= glm::mat4(this->orientation);
//	this->matWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(this->scale, this->scale, this->scale));
//	this->inverseTransposeMatWorld = glm::inverse(glm::transpose(this->matWorld));
//}

void cMeshComponent::preFrame(float dt, float tt)
{
}

void cMeshComponent::render(float dt, float tt)
{
	cShaderManager::cShaderProgram* pShader = cShaderManager::getCurrentShader();

	sModelDrawInfo drawInfo;
	if (cVAOManager::getCurrentVAOManager()->FindDrawInfoByModelName(this->useLOD ? this->lodMeshName : this->meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void cMeshComponent::instatiateBaseVariables(const Json::Value& obj)
{
	this->meshName = obj["meshName"] ? obj["meshName"].asString() : "";
	this->scale = obj["scale"] ? obj["scale"].asFloat() : 1.0f;

	if (obj["LOD"])
	{
		this->lodMeshName = obj["LOD"]["meshName"] ? obj["LOD"]["meshName"].asString() : "";
		this->lodScale = obj["LOD"]["scale"] ? obj["LOD"]["scale"].asFloat() : 1.0f;
		this->lodDistance = obj["LOD"]["distance"] ? obj["LOD"]["distance"].asFloat() : 100.0f;
	}
}

void cMeshComponent::serializeJSONObject(Json::Value& obj)
{

}