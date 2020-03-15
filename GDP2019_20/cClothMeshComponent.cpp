#include "cClothMeshComponent.hpp"
#include "cShaderManager.hpp"
#include "cVAOManager.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

cClothMeshComponent::cClothMeshComponent()
{
}

cClothMeshComponent::~cClothMeshComponent()
{
}

void cClothMeshComponent::init()
{
}

void cClothMeshComponent::preFrame(float dt, float tt)
{
}

void cClothMeshComponent::update(float dt, float tt)
{
}

void cClothMeshComponent::render(float dt, float tt)
{
	cShaderManager::cShaderProgram* pShader = cShaderManager::getCurrentShader();

	sModelDrawInfo drawInfo;
	if (!cVAOManager::getCurrentVAOManager()->FindDrawInfoByModelName("sphere", drawInfo))
		return;

	glBindVertexArray(drawInfo.VAO_ID);

	size_t numNodes = cloth->NumNodes();
	glm::vec3 position(0.0f);
	glm::mat4 m(1.0f);
	float rad = 1.0f;
	this->cloth->GetNodeRadius(0, rad);
	for (unsigned i = 0; i < numNodes; ++i)
	{
		cloth->GetNodePosition(i, position);
		m = glm::translate(glm::mat4(1.0f), position);
		m *= glm::scale(glm::mat4(1.0f), glm::vec3(rad * 2.0f));

		glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
		glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	
}

eComponentType cClothMeshComponent::getType()
{
	return eComponentType();
}

void cClothMeshComponent::instatiateBaseVariables(const Json::Value& obj)
{
}

void cClothMeshComponent::serializeJSONObject(Json::Value& obj)
{
}
