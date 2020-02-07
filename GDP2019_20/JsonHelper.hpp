#pragma once

#include <json/json.h>

#include <json/json.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Json
{
	inline Value fromVec3(const glm::vec3& v)
	{
		return Value().append(v[0]).append(v[1]).append(v[2]);
	}

	inline glm::vec3 toVec3(const Value& v)
	{
		return glm::vec3(v[0].asFloat(), v[1].asFloat(), v[2].asFloat());
	}

	inline Value fromVec4(const glm::vec4& v)
	{
		return Value().append(v[0]).append(v[1]).append(v[2]).append(v[3]);
	}

	inline glm::vec4 toVec4(const Value& v)
	{
		return glm::vec4(v[0].asFloat(), v[1].asFloat(), v[2].asFloat(), v[3].asFloat());
	}
}