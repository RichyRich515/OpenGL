#pragma once

#include <glm/vec3.hpp>
#include <btBulletDynamicsCommon.h>

namespace nConvert
{
	inline btVector3 ToBullet(const glm::vec3& v)
	{
		return btVector3(v.x, v.y, v.z);
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut)
	{
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}
}