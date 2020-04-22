#pragma once

#include <vector>
#include <string>

#include "cParticle.hpp"
#include "iGameObject.hpp"

template <class T>
T randInRange(T min, T max)
{
	double value =
		min + static_cast <double> (rand())
		/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
	return static_cast<T>(value);
};

class cParticleEmitter // : public cGameObject
{
private:
	std::vector<cParticle*> vecParticles;
	static const std::size_t DEFAULT_MAX_PARTICLES = 1000;

public:
	cParticleEmitter();

	std::string meshName;
	iGameObject* parentObject = nullptr;
	glm::vec3 parentOffset;

	bool active;
	bool spawnNew;

	glm::vec3 position;
	glm::vec3 eyePosition; // For Imposters

	glm::vec3 particleAcceleration;
	glm::vec3 particleInitialVelocityMin;
	glm::vec3 particleInitialVelocityMax;

	float particleLifeMin;
	float particleLifeMax;
	float startScale;
	float endScale;
	glm::vec3 positionOffsetMin;
	glm::vec3 positionOffsetMax;
	std::size_t newPerUpdateMin;
	std::size_t newPerUpdateMax;

	glm::vec4 startColor;
	glm::vec4 endColor;

	//glm::quat startQOrientation;
	//glm::quat endQOrientation;


	void init(glm::vec3 pos, glm::vec3 particleAccel,
		glm::vec3 minInitVel, glm::vec3 maxInitVel,
		glm::vec3 minDeltaPos, glm::vec3 maxDeltaPos,
		float minLife, float maxLife,
		glm::vec4 startCol, glm::vec4 endCol,
		float startScale, float endScale,
		std::size_t minNewParticlesPerUpdate, std::size_t maxNewParticlesPerUpdate,
		std::size_t maxParticles);


	void update(float dt, float tt);
	void render(float dt, float tt);
	
	// For drawing
	void getParticles(std::vector<cParticle*>& vecParticles, glm::vec3 eyePosition = glm::vec3(0.0f, 0.0f, 0.0f), bool isImposter = false);
private:
	glm::quat m_calcImposterRotationFromEye(glm::vec3 particlePos);

	bool createNewParticle();
};