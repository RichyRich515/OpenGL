#include "cParticleEmitter.hpp"
#include "cShaderManager.hpp"
#include "cVAOManager.hpp"
#include "GLCommon.h"

cParticleEmitter::cParticleEmitter()
{
	active = true;
	spawnNew = true;
}

void cParticleEmitter::init(glm::vec3 pos, glm::vec3 particleAccel,
	glm::vec3 minInitVel, glm::vec3 maxInitVel,
	glm::vec3 minDeltaPos, glm::vec3 maxDeltaPos,
	float minLife, float maxLife,
	glm::vec4 startCol, glm::vec4 endCol,
	float startScale, float endScale,
	std::size_t minNewParticlesPerUpdate, std::size_t maxNewParticlesPerUpdate, 
	std::size_t maxParticles = cParticleEmitter::DEFAULT_MAX_PARTICLES)
{
	//if (minNewParticlesPerUpdate > maxParticles || maxNewParticlesPerUpdate > maxParticles)
	//{
	//	// Cannot make more particles per frame than total particles
	//	return;
	//}

	// reserve just sets up space, does not create objects
	this->vecParticles.reserve(maxParticles);
	//this->vecParticles.clear();
	for (std::size_t i = 0; i < maxParticles; ++i)
	{
		cParticle* p = new cParticle();
		p->remainingLife = 0.0f;
		this->vecParticles.push_back(p);
	}
	this->position = pos;
	this->particleAcceleration = particleAccel;
	this->particleInitialVelocityMin = minInitVel;
	this->particleInitialVelocityMax = maxInitVel;
	this->positionOffsetMin = minDeltaPos;
	this->positionOffsetMax = maxDeltaPos;
	this->particleLifeMin = minLife;
	this->particleLifeMax = maxLife;
	this->startColor = startCol;
	this->endColor = endCol;
	this->startScale = startScale;
	this->endScale = endScale;
	this->newPerUpdateMin = minNewParticlesPerUpdate;
	this->newPerUpdateMax = maxNewParticlesPerUpdate;
}

void cParticleEmitter::update(float dt, float tt)
{
	if (!active)
		return;

	if (this->parentObject)
	{
		this->position = this->parentObject->getPosition() + this->parentOffset;
	}


	if (spawnNew)
	{
		for (int count = randInRange(this->newPerUpdateMin, this->newPerUpdateMax); count > 0; --count)
		{
			if (!this->createNewParticle())
				break; // No more open slots in pool
		}
	}

	for (cParticle* p : vecParticles)
	{
		p->remainingLife -= dt;
		if (p->remainingLife >= 0.0f)
		{
			float percentToDeath = p->remainingLife / p->maxLife;
			p->velocity += this->particleAcceleration * dt;
			p->position += p->velocity * dt;
			p->color = glm::mix(endColor, startColor, percentToDeath);
			p->scale = glm::mix(endScale, startScale, percentToDeath);
		}
	}

}

void cParticleEmitter::render(float dt, float tt)
{
	if (!this->active)
		return;
	
	if (this->meshName.empty())
		this->meshName = "sphere";

	sModelDrawInfo info;
	cVAOManager::getCurrentVAOManager()->FindDrawInfoByModelName(meshName, info);

	auto pShader = cShaderManager::getCurrentShader();

	glUniform4f(pShader->getUniformLocID("specularColour"), 0.0f, 0.0f, 0.0f, 0.0f);// TODO: figure out specular?
	glUniform4f(pShader->getUniformLocID("params1"), dt, tt, 1.0f, 0.0f);
	glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);
	glUniform4f(pShader->getUniformLocID("textparams00"), 0.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(info.VAO_ID);

	glm::mat4 m;
	for (auto p : this->vecParticles)
	{
		if (p->remainingLife > 0.0f && p->scale > 0.0f)
		{
			m = glm::translate(glm::mat4(1.0f), p->position);
			m *= glm::scale(glm::mat4(1.0f), glm::vec3(p->scale));

			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(m));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(m))));
			
			glUniform4f(pShader->getUniformLocID("diffuseColour"), p->color.r, p->color.g, p->color.b, p->color.a);
			glDrawElements(GL_TRIANGLES, info.numberOfIndices, GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);
}

void cParticleEmitter::getParticles(std::vector<cParticle*>& vecParticles, glm::vec3 eyePosition, bool isImposter)
{

	vecParticles.clear();
	vecParticles.reserve(this->vecParticles.size());

	for (cParticle* p : this->vecParticles)
	{
		if (p->remainingLife >= 0)
		{
			if (isImposter)
			{
				p->qOrientation = m_calcImposterRotationFromEye(p->position);
			}
			vecParticles.push_back(p);
		}
	}
}

bool cParticleEmitter::createNewParticle()
{
	for (cParticle* p : vecParticles)
	{
		if (p->remainingLife <= 0.0f)
		{
			p->maxLife = randInRange(this->particleLifeMin, this->particleLifeMax);
			p->remainingLife = p->maxLife;

			p->position.x = randInRange(this->positionOffsetMin.x, positionOffsetMax.x) + this->position.x;
			p->position.y = randInRange(this->positionOffsetMin.y, positionOffsetMax.y) + this->position.y;
			p->position.z = randInRange(this->positionOffsetMin.z, positionOffsetMax.z) + this->position.z;

			p->velocity.x = randInRange(this->particleInitialVelocityMin.x, this->particleInitialVelocityMax.x);
			p->velocity.y = randInRange(this->particleInitialVelocityMin.y, this->particleInitialVelocityMax.y);
			p->velocity.z = randInRange(this->particleInitialVelocityMin.z, this->particleInitialVelocityMax.z);


			p->color = startColor;
			return true;
		}
	}
	return false;
}

glm::quat cParticleEmitter::m_calcImposterRotationFromEye(glm::vec3 particlePos)
{
	// http://www.rastertek.com/dx11tut34.html
	//     // Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
	// angle = atan2(modelPosition.x - cameraPosition.x,
	//               modelPosition.z - cameraPosition.z)
	//                        * (180.0 / D3DX_PI);

	// Convert rotation into radians.
	// rotation = (float)angle * 0.0174532925f

	float angle = atan2(particlePos.x - this->eyePosition.x,
						particlePos.z - eyePosition.z);
	//* ( 180.0 / PI );
	// Since we aren't paying any attention to the x and z rotations.
	return glm::vec3(0.0f, angle, 0.0f);
}