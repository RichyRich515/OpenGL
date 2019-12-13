#pragma once

#include <vector>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Forward declaration
class cGameObject;

class iCommand
{
private:
	bool initialized = false;
	bool done = false;

	std::vector<iCommand*> parallels;
	std::vector<iCommand*> serials;
protected:
	cGameObject* go;
	bool easeIn;
	bool easeOut;

	virtual bool my_update(float dt, float tt) = 0;
	virtual void my_init(float dt, float tt) = 0;
public:
	~iCommand();
	void update(float dt, float tt);
	void init(float dt, float tt);
	bool isDone() { return done; }
	bool isInit() { return initialized; }


	void addParallel(iCommand* cmd) { this->parallels.push_back(cmd); }
	void addSerial(iCommand* cmd) { this->serials.push_back(cmd); }
};


class cCommand_Wait : public iCommand
{
protected:
	float duration;
public:
	cCommand_Wait(cGameObject* go, float duration);

	virtual void my_init(float dt, float tt);
	virtual bool my_update(float dt, float tt);
};


class cCommand_SetPosition : public iCommand
{
protected:
	glm::vec3 position;
public:
	cCommand_SetPosition(cGameObject* go, glm::vec3 position);

	virtual void my_init(float dt, float tt);
	virtual bool my_update(float dt, float tt);
};


class cCommand_SetVisible : public iCommand
{
protected:
	bool visibility;
public:
	cCommand_SetVisible(cGameObject* go, bool visible);

	virtual void my_init(float dt, float tt);
	virtual bool my_update(float dt, float tt);
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