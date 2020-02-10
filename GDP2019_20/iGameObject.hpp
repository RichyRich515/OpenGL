#pragma once

#include "iMessageable.hpp"
#include "iComponent.hpp"

class iGameObject : public iMessageable, public iComponent
{
public:
	unsigned id;

	std::string name;
	std::string type;

	virtual ~iGameObject() {}

	virtual glm::vec3 getPosition() = 0;
};