#include "cBoidGameObject.hpp"
#include "cSteeringFunctions.hpp"

cBoidGameObject::cBoidGameObject()
{
}

cBoidGameObject::cBoidGameObject(std::string name)
{
}

cBoidGameObject::cBoidGameObject(Json::Value& obj)
{
}

cBoidGameObject::cBoidGameObject(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
}

cBoidGameObject::~cBoidGameObject()
{
}

eComponentType cBoidGameObject::getType()
{
	return eComponentType();
}


glm::vec3 cBoidGameObject::getPosition()
{
	return glm::vec3();
}


void cBoidGameObject::init()
{
}


void cBoidGameObject::preFrame()
{
}


void cBoidGameObject::update(float dt, float tt)
{
	glm::vec3 destination = this->coordinator->position + this->coordinator->offsets[id];
	glm::vec3 steer = cSteeringFunctions::steerSeekArrive(this->transform.position, destination, this->velocity, 4.0f, this->max_speed, dt);
	if (glm::length(steer) > 0.0f)
		this->velocity += steer;

	this->transform.position += this->velocity * dt;
	if (glm::length(this->velocity) > 0.0f && glm::distance(this->transform.position, destination) > 0.1f)
		this->transform.orientation = glm::quatLookAt(glm::normalize(-this->velocity), glm::vec3(0.0f, 1.0f, 0.0f));

	this->transform.updateMatricis();
}


void cBoidGameObject::render()
{
	if (!this->graphics.visible)
		return;

	glm::mat4 m = this->transform.matWorld;
	m *= glm::scale(glm::mat4(1.0), glm::vec3(mesh.scale));
	cShaderManager::cShaderProgram* pShader = cShaderManager::getCurrentShader();
	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	this->graphics.render();
	this->mesh.render();
}


sMessage cBoidGameObject::message(sMessage const& msg)
{
	return sMessage();
}


void cBoidGameObject::instatiateBaseVariables(const Json::Value& obj)
{
}


void cBoidGameObject::instatiateUniqueVariables(const Json::Value& obj)
{
}

void cBoidGameObject::serializeJSONObject(Json::Value& obj)
{
}

void cBoidGameObject::serializeUniqueVariables(Json::Value& obj)
{
}
