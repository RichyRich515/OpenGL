#include "cAnimatedGameObject.hpp"
#include "cShaderManager.hpp"
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cAnimatedGameObject::getPosition()
{
	return this->transform.getPosition();
}

sMessage cAnimatedGameObject::message(sMessage const& msg)
{
	return sMessage();
}

void cAnimatedGameObject::init()
{
}

cAnimatedGameObject::cAnimatedGameObject()
{
}

eComponentType cAnimatedGameObject::getType()
{
	return eComponentType::GameObject;
}

void cAnimatedGameObject::preFrame()
{
}

void cAnimatedGameObject::render()
{
	if (!this->graphics.visible)
		return;

	auto pShader = cShaderManager::getCurrentShader();
	glUniform1f(pShader->getUniformLocID("isSkinnedMesh"), (float)GL_TRUE);

	glm::mat4 m = this->transform.matWorld;
	m *= glm::scale(glm::mat4(1.0), glm::vec3(mesh.scale));
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render();
	this->skinmesh.render();
	this->mesh.render();

	glUniform1f(pShader->getUniformLocID("isSkinnedMesh"), (float)GL_FALSE);
}

void cAnimatedGameObject::update(float dt, float tt)
{
	this->graphics.update(dt, tt);
	this->skinmesh.update(dt, tt);
	this->mesh.update(dt, tt);
	this->transform.update(dt, tt);
}
