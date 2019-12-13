#include "cCommand.hpp"
#include "cGameObject.hpp"
#include "cWorld.hpp"

iCommand::~iCommand()
{
	for (iCommand* cmd : this->parallels)
	{
		if (cmd)
			delete cmd;
	}
	for (iCommand* cmd : this->serials)
	{
		if (cmd)
			delete cmd;
	}
}

void iCommand::update(float dt, float tt)
{
	bool d = this->my_update(dt, tt);

	// run parallel commands
	for (iCommand* cmd : this->parallels)
	{
		if (!cmd->isDone())
		{
			cmd->update(dt, tt);
			d = false;
		}
	}

	if (d)
	{
		// done current command, go to next
		for (iCommand* cmd : this->serials)
		{
			if (!cmd->isInit())
				cmd->init(dt, tt);

			if (!cmd->isDone())
			{
				cmd->update(dt, tt);
				d = false;
			}
		}

		// done all serial commands as well
		if (d)
		{
			this->done = true;
		}
	}
}
void iCommand::init(float dt, float tt)
{
	this->initialized = true;
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

cCommand_Wait::cCommand_Wait(cGameObject* go, float duration)
{
	this->go = go;
	this->duration = duration;
}
void cCommand_Wait::my_init(float dt, float tt)
{
}
bool cCommand_Wait::my_update(float dt, float tt)
{
	if (this->duration > 0.0f)
		this->duration -= dt;
	else
		return true;
	return false;
}

cCommand_SetPosition::cCommand_SetPosition(cGameObject* go, glm::vec3 position)
{
	this->go = go;
	this->position = position;
}
void cCommand_SetPosition::my_init(float dt, float tt)
{
	this->go->position = this->position;
}
bool cCommand_SetPosition::my_update(float dt, float tt)
{
	return true;
}

cCommand_SetVisible::cCommand_SetVisible(cGameObject* go, bool visible)
{
	this->go = go;
	this->visibility = visible;
}
void cCommand_SetVisible::my_init(float dt, float tt)
{
	this->go->visible = this->visibility;
}
bool cCommand_SetVisible::my_update(float dt, float tt)
{
	return true;
}
