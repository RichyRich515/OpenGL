#pragma once

#include <glm/vec3.hpp>
#include <string>

struct sMessage
{
public:
	std::string name;
	union uData
	{
		bool b;
		int i;
		float f;
		glm::vec3 v3;
		void* vp;
	} data;
};

class iMessageable
{
public:
	virtual sMessage message(sMessage const& msg) = 0;
};