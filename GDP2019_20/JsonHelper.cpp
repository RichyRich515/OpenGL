#include "JsonHelper.hpp"

namespace Json
{
	Value fromVec3(const glm::vec3& v)
	{
		return Value().append(v[0]).append(v[1]).append(v[2]);
	}

	glm::vec3 toVec3(const Value& v)
	{
		return glm::vec3(v[0], v[1], v[2]);
	}

	Value fromVec4(const glm::vec4& v)
	{
		return Value().append(v[0]).append(v[1]).append(v[2]).append(v[3]);
	}

	glm::vec4 toVec4(const Value& v)
	{
		return glm::vec4(v[0], v[1], v[2], v[3]);
	}
}
