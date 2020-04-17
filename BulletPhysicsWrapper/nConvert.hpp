#pragma once
#include <glm/vec3.hpp>
#include <btBulletDynamicsCommon.h>

namespace nConvert
{
	inline void ToSimple(const btVector3& vectorIn, glm::vec3& vectorOut)
	{
		vectorOut.x = vectorIn[0];
		vectorOut.y = vectorIn[1];
		vectorOut.z = vectorIn[2];
	}

	inline btVector3 ToBullet(const glm::vec3& v)
	{
		return btVector3(v.x, v.y, v.z);
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut)
	{
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}
}