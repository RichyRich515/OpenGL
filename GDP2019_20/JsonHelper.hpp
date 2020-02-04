#pragma once

#include <json/json.h>

#include <json/json.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Json
{
	inline Value fromVec3(const glm::vec3& v);
	inline glm::vec3 toVec3(const Value& v);

	inline Value fromVec4(const glm::vec4& v);
	inline glm::vec4 toVec4(const Value& v);
}