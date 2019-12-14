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


cCommand_MoveToTimed::cCommand_MoveToTimed(cGameObject* go, float duration, glm::vec3 destination, bool easeIn, bool easeOut)
{
	this->go = go;
	this->duration = 0.0f;
	this->max_duration = duration;
	this->startposition = glm::vec3(0.0f);
	this->destination = destination;
	this->full_translation = glm::vec3(0.0f);
	this->easeIn = easeIn;
	this->easeOut = easeOut;
}
void cCommand_MoveToTimed::my_init(float dt, float tt)
{
	this->startposition = go->position;
	this->full_translation = this->destination - this->startposition;
}
bool cCommand_MoveToTimed::my_update(float dt, float tt)
{
	if (this->duration < this->max_duration)
		this->duration += dt;
	else
		return true;

	if (this->duration > this->max_duration)
		this->duration = this->max_duration;

	// Parametric blend https://stackoverflow.com/a/25730573
	float t = this->duration / this->max_duration;
	float t2 = t * t;
	glm::vec3 offset;
	if ((easeIn && t <= 0.5f) || (easeOut && t > 0.5f))
	{
		offset = this->full_translation * (t2 / (2.0f * (t2 - t) + 1.0f));
	}
	else
	{
		offset = this->full_translation * t;
	}
	// todo: maybe use a lerp here instead? might be faster
	this->go->position = this->startposition + offset;
	return false;
}


cCommand_RotateToTimed::cCommand_RotateToTimed(cGameObject* go, float duration, glm::vec3 rotation, bool easeIn, bool easeOut)
{
	this->go = go;
	this->duration = 0.0f;
	this->max_duration = duration;
	this->startOrientation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	this->endOrientation = glm::quat(rotation);
	this->easeIn = easeIn;
	this->easeOut = easeOut;
}
void cCommand_RotateToTimed::my_init(float dt, float tt)
{
	this->startOrientation = go->qOrientation;
}
bool cCommand_RotateToTimed::my_update(float dt, float tt)
{
	if(this->duration < max_duration)
		this->duration += dt;
	else
		return true;
	if (this->duration > this->max_duration)
		this->duration = this->max_duration;

	// Parametric blend https://stackoverflow.com/a/25730573
	float t = this->duration / this->max_duration;
	float t2 = t * t;
	
	float amt = t;
	if ((easeIn && t <= 0.5f) || (easeOut && t > 0.5f))
	{
		amt = (t2 / (2.0f * (t2 - t) + 1.0f));
	}

	this->go->qOrientation = glm::slerp(this->startOrientation, this->endOrientation, amt);
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
