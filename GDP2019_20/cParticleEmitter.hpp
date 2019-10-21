#pragma once

#include <vector>

#include "cParticle.hpp"

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

	glm::vec3 position;
	glm::vec3 particleAcceleration;
	glm::vec3 particleInitialVelocityMin;
	glm::vec3 particleInitialVelocityMax;

	float particleLifeMin;
	float particleLifeMax;
	glm::vec3 positionOffsetMin;
	glm::vec3 positionOffsetMax;
	std::size_t newPerUpdateMin;
	std::size_t newPerUpdateMax;

	glm::vec4 startColor;
	glm::vec4 endColor;

	// glm::vec3 orientation


	void init(glm::vec3 pos, glm::vec3 particleAccel,
		glm::vec3 minInitVel, glm::vec3 maxInitVel,
		glm::vec3 minDeltaPos, glm::vec3 maxDeltaPos,
		float minLife, float maxLife,
		glm::vec4 startCol, glm::vec4 endCol,
		std::size_t minNewParticlesPerUpdate, std::size_t maxNewParticlesPerUpdate,
		std::size_t maxParticles);


	void update(float dt);

	// For drawing
	void getParticles(std::vector<cParticle*>& vecParticles);
private:

	bool createNewParticle();
};