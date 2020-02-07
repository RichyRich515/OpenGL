//#include "cCommand.hpp"
//#include "cGameObject.hpp"
//#include "cWorld.hpp"
//
//iCommand::~iCommand()
//{
//	for (iCommand* cmd : this->parallels)
//	{
//		if (cmd)
//			delete cmd;
//	}
//	for (iCommand* cmd : this->serials)
//	{
//		if (cmd)
//			delete cmd;
//	}
//}
//
//void iCommand::update(float dt, float tt)
//{
//	bool d = this->my_update(dt, tt);
//
//	// run parallel commands
//	for (iCommand* cmd : this->parallels)
//	{
//		if (!cmd->isDone())
//		{
//			cmd->update(dt, tt);
//			d = false;
//		}
//	}
//
//	if (d)
//	{
//		// done current command, go to next
//		for (iCommand* cmd : this->serials)
//		{
//			if (!cmd->isInit())
//				cmd->init(dt, tt);
//
//			if (!cmd->isDone())
//			{
//				cmd->update(dt, tt);
//				d = false;
//			}
//		}
//
//		// done all serial commands as well
//		if (d)
//		{
//			this->done = true;
//		}
//	}
//}
//void iCommand::init(float dt, float tt)
//{
//	this->initialized = true;
//	this->my_init(dt, tt);
//	for (iCommand* cmd : this->parallels)
//	{
//		cmd->init(dt, tt);
//	}
//}
//
//
//cCommand_MoveToTimed::cCommand_MoveToTimed(cGameObject* go, float duration, glm::vec3 destination, bool easeIn, bool easeOut)
//{
//	this->go = go;
//	this->duration = 0.0f;
//	this->max_duration = duration;
//	this->startposition = glm::vec3(0.0f);
//	this->destination = destination;
//	this->full_translation = glm::vec3(0.0f);
//	this->easeIn = easeIn;
//	this->easeOut = easeOut;
//}
//void cCommand_MoveToTimed::my_init(float dt, float tt)
//{
//	this->startposition = go->getPosition();
//	this->full_translation = this->destination - this->startposition;
//}
//bool cCommand_MoveToTimed::my_update(float dt, float tt)
//{
//	if (this->duration < this->max_duration)
//		this->duration += dt;
//	else
//		return true;
//
//	if (this->duration > this->max_duration)
//		this->duration = this->max_duration;
//
//	// Parametric blend https://stackoverflow.com/a/25730573
//	float t = this->duration / this->max_duration;
//	float t2 = t * t;
//	glm::vec3 offset;
//	if ((easeIn && t <= 0.5f) || (easeOut && t > 0.5f))
//	{
//		offset = this->full_translation * (t2 / (2.0f * (t2 - t) + 1.0f));
//	}
//	else
//	{
//		offset = this->full_translation * t;
//	}
//	this->go->setPosition(this->startposition + offset);
//
//	cWorld* world = cWorld::getWorld();
//	if (world->debugMode)
//	{
//		world->pDebugRenderer->addLine(startposition, destination, glm::vec3(1.0f, 1.0f, 0.0f), 0.0f);
//	}
//
//	return false;
//}
//
//
//cCommand_RotateToTimed::cCommand_RotateToTimed(cGameObject* go, float duration, glm::vec3 rotation, bool easeIn, bool easeOut)
//{
//	this->go = go;
//	this->duration = 0.0f;
//	this->max_duration = duration;
//	this->startOrientation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
//	this->endOrientation = glm::quat(rotation);
//	this->easeIn = easeIn;
//	this->easeOut = easeOut;
//}
//void cCommand_RotateToTimed::my_init(float dt, float tt)
//{
//	this->startOrientation = go->getOrientation();
//}
//bool cCommand_RotateToTimed::my_update(float dt, float tt)
//{
//	if(this->duration < max_duration)
//		this->duration += dt;
//	else
//		return true;
//	if (this->duration > this->max_duration)
//		this->duration = this->max_duration;
//
//	// Parametric blend https://stackoverflow.com/a/25730573
//	float t = this->duration / this->max_duration;
//	float t2 = t * t;
//	
//	float amt = t;
//	if ((easeIn && t <= 0.5f) || (easeOut && t > 0.5f))
//	{
//		amt = (t2 / (2.0f * (t2 - t) + 1.0f));
//	}
//
//	cWorld* world = cWorld::getWorld();
//	if (world->debugMode)
//	{
//		glm::vec3 pos = this->go->getPosition();
//		world->pDebugRenderer->addLine(pos, pos + this->startOrientation * glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.0f), 0.0f);
//		world->pDebugRenderer->addLine(pos, pos + this->endOrientation * glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.5f, 1.0f), 0.0f);
//	}
//
//	this->go->setOrientation(glm::slerp(this->startOrientation, this->endOrientation, amt));
//	return false;
//}
//
//cCommand_Wait::cCommand_Wait(cGameObject* go, float duration)
//{
//	this->go = go;
//	this->duration = duration;
//}
//void cCommand_Wait::my_init(float dt, float tt)
//{
//}
//bool cCommand_Wait::my_update(float dt, float tt)
//{
//	if (this->duration > 0.0f)
//		this->duration -= dt;
//	else
//		return true;
//	return false;
//}
//
//cCommand_SetPosition::cCommand_SetPosition(cGameObject* go, glm::vec3 position)
//{
//	this->go = go;
//	this->position = position;
//}
//void cCommand_SetPosition::my_init(float dt, float tt)
//{
//	this->go->setPosition(this->position);
//}
//bool cCommand_SetPosition::my_update(float dt, float tt)
//{
//	return true;
//}
//
//cCommand_SetVisible::cCommand_SetVisible(cGameObject* go, bool visible)
//{
//	this->go = go;
//	this->visibility = visible;
//}
//void cCommand_SetVisible::my_init(float dt, float tt)
//{
//	this->go->visible = this->visibility;
//}
//bool cCommand_SetVisible::my_update(float dt, float tt)
//{
//	return true;
//}
//
//glm::vec3 getPoint(glm::vec3 p1, glm::vec3 p2, float t)
//{
//	glm::vec3 diff = p2 - p1;
//	return p1 + (diff * t);
//}
//cCommand_MoveCurveTimed::cCommand_MoveCurveTimed(cGameObject* go, float duration, glm::vec3 end, glm::vec3 point, bool easeIn, bool easeOut)
//{
//	this->go = go;
//	this->duration = 0.0f;
//	this->max_duration = duration;
//	this->start = glm::vec3(0.0f);
//	this->end = end;
//	this->point = point;
//	this->easeIn = easeIn;
//	this->easeOut = easeOut;
//}
//void cCommand_MoveCurveTimed::my_init(float dt, float tt)
//{
//	this->start = this->go->getPosition();
//}
//bool cCommand_MoveCurveTimed::my_update(float dt, float tt)
//{
//	if (this->duration < this->max_duration)
//		this->duration += dt;
//	else
//		return true;
//
//	if (this->duration > this->max_duration)
//		this->duration = this->max_duration;
//
//	// Parametric blend https://stackoverflow.com/a/25730573
//	float t = this->duration / this->max_duration;
//	float t2 = t * t;
//	if ((easeIn && t <= 0.5f) || (easeOut && t > 0.5f))
//	{
//		t = (t2 / (2.0f * (t2 - t) + 1.0f));
//	}
//
//	// BONUS: MATHEMATICAL CURVE!!
//	glm::vec3 pa = getPoint(this->start, this->point, t);
//	glm::vec3 pb = getPoint(this->point, this->end, t);
//
//	this->go->setPosition(getPoint(pa, pb, t));
//
//	cWorld* world = cWorld::getWorld();
//	if (world->debugMode)
//	{
//		world->pDebugRenderer->addLine(start, end, glm::vec3(1.0f, 1.0f, 0.0f), 0.0f);
//		world->pDebugRenderer->addLine(start, point, glm::vec3(0.0f, 1.0f, 1.0f), 0.0f);
//		world->pDebugRenderer->addLine(point, end, glm::vec3(1.0f, 0.0f, 1.0f), 0.0f);
//	}
//
//	return false;
//}
//
//
//cCommand_FollowTimed::cCommand_FollowTimed(cGameObject* go, cGameObject* target, float duration, float speed, float minDistance, float maxDistance, glm::vec3 offset, bool realtiveToOrientation)
//{
//	this->go = go;
//	this->target = target;
//	this->duration = duration;
//	this->speed = speed;
//	this->minDistance = minDistance;
//	this->maxDistance = maxDistance;
//	this->offset = offset;
//	this->relativeToOrientation = relativeToOrientation;
//}
//void cCommand_FollowTimed::my_init(float dt, float tt)
//{
//	this->diffMinAndMax = this->maxDistance - this->minDistance;
//}
//bool cCommand_FollowTimed::my_update(float dt, float tt)
//{
//	if (this->duration > 0.0f)
//		this->duration -= dt;
//	else
//		return true;
//
//	// Adapted from Feeney's follow cam code
//
//	glm::vec3 idealPos = this->target->getPosition() + this->offset;
//	glm::vec3 direction = glm::normalize(idealPos - this->go->getPosition());
//	float distanceToIdeal = glm::distance(this->go->getPosition(), idealPos);
//
//	// Set to maximum speed by default;
//	glm::vec3 vel = direction * this->speed;
//
//	
//	if (distanceToIdeal < this->minDistance)
//	{
//		// too close stop moving
//		vel = glm::vec3(0.0f, 0.0f, 0.0f);
//	}
//	else if (!(distanceToIdeal > this->maxDistance)) 
//	{
//		// inbetween min and max range
//		vel *= glm::smoothstep(0.0f, 1.0f, (distanceToIdeal - this->minDistance) / diffMinAndMax);
//	}
//
//	this->go->setPosition(this->go->getPosition() + vel * dt);
//
//	return false;
//}
