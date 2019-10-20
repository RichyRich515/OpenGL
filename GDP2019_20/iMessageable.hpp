#pragma once

#include <glm/vec3.hpp>
#include <string>

struct sMessage
{
public:
	sMessage() : name("") { }
	sMessage(std::string namestr) : name(namestr) { }
	sMessage(std::string namestr, bool b) : name(namestr) { data.b = b; }
	sMessage(std::string namestr, int i) : name(namestr) { data.i = i; }
	sMessage(std::string namestr, float f) : name(namestr) { data.f = f; }
	sMessage(std::string namestr, glm::vec3 v3) : name(namestr) { data.v3 = v3; }
	sMessage(std::string namestr, void* vp) : name(namestr) { data.vp = vp; }

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