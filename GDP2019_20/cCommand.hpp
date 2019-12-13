#pragma once

#include <vector>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Forward declaration
class cGameObject;

enum class eCommandType
{
	Error = -1,
	Nothing = 0,
	MoveToTimed = 1,
	MoveToSpeed = 2,
	RotateToTimed = 3,
	RotateToSpeed = 4,
	FollowCurve = 5,
	FollowObject = 6,
};

class iCommand
{
protected:
	eCommandType type;
	cGameObject* go;
	bool done;
	bool easeIn;
	bool easeOut;
	std::vector<iCommand*> parallels;
	std::vector<iCommand*> serials;

	virtual bool my_update(float dt, float tt) = 0;
	virtual void my_init(float dt, float tt) = 0;
public:

	void update(float dt, float tt);
	void init(float dt, float tt);
	bool isDone() { return done; }


	void addParallel(iCommand* cmd) { this->parallels.push_back(cmd); }
	void addSerial(iCommand* cmd) { this->serials.push_back(cmd); }
};


class cCommand_MoveToTimed : public iCommand
{
protected:
	float duration;
	glm::vec3 destination;

	glm::vec3 velocity; // calculated on init
public:
	cCommand_MoveToTimed(cGameObject* go, float duration, glm::vec3 destination);

	virtual void my_init(float dt, float tt);
	virtual bool my_update(float dt, float tt);
};


class cCommand_RotateToTimed : public iCommand
{
protected:
	float duration;
	float duration_max;
	glm::quat startOrientation;
	glm::quat endOrientation;
public:
	cCommand_RotateToTimed(cGameObject* go, float duration, glm::vec3 rotation);

	virtual void my_init(float dt, float tt);
	virtual bool my_update(float dt, float tt);
};