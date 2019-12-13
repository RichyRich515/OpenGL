#include "cCommand.hpp"
#include "cGameObject.hpp"
#include "cWorld.hpp"

void iCommand::update(float dt, float tt)
{
	bool d = false;
	if (this->my_update(dt, tt))
	{
		d = true;
	}

	// run parallel commands
	for (iCommand* cmd : this->parallels)
	{
		cmd->update(dt, tt);
		if (!cmd->isDone())
			d = false;
	}

	if (d)
	{
		// done
		// move to next serial command
	}
}
void iCommand::init(float dt, float tt)
{
	this->my_init(dt, tt);
	for (iCommand* cmd : this->parallels)
	{
		cmd->init(dt, tt);
	}
}


cCommand_MoveToTimed::cCommand_MoveToTimed(cGameObject* go, float duration, glm::vec3 destination)
{
	this->go = go;
	this->duration = duration;
	this->destination = destination;
	this->velocity = glm::vec3(0.0f);
}
void cCommand_MoveToTimed::my_init(float dt, float tt)
{
	this->velocity = glm::normalize(destination - this->go->position);
	this->velocity *= glm::distance(this->go->position, destination) / duration;
}
bool cCommand_MoveToTimed::my_update(float dt, float tt)
{
	if (this->duration > 0.0f)
		this->duration -= dt;
	else
		return true;
		
	this->go->position += this->velocity * dt;
	return false;
}

cCommand_RotateToTimed::cCommand_RotateToTimed(cGameObject* go, float duration, glm::vec3 rotation)
{
	this->go = go;
	this->duration = 0.0f;
	this->duration_max = duration;
	this->startOrientation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	this->endOrientation = glm::quat(rotation);
}
void cCommand_RotateToTimed::my_init(float dt, float tt)
{
	this->startOrientation = go->qOrientation;
}
bool cCommand_RotateToTimed::my_update(float dt, float tt)
{
	if(this->duration < duration_max)
		this->duration += dt;
	else
		return true;
	if (this->duration > this->duration_max)
		this->duration = this->duration_max;
	this->go->qOrientation = glm::slerp(this->startOrientation, this->endOrientation, duration / duration_max);
	return false;
}
