#include "cPelican.hpp"

cPelican::cPelican()
{

}

cPelican::cPelican(std::string name, glm::vec3 startingPos)
{

}

cPelican::cPelican(Json::Value& obj, std::map<std::string, cMesh*>& mapMeshes)
{
	instatiateBaseVariables(obj, mapMeshes);
	instatiateUniqueVariables(obj);

	this->maxRotationSpeed = 1.25f;
	this->maxSpeed = 50.0f;
	this->forwardDecel = 10.0f;
	this->updownDecel = 10.0f;
	this->leftrightDecel = 2.5f;
	this->rollDecel = 2.5f;
}

void cPelican::init()
{

}

void cPelican::update(float dt)
{
	glm::vec3 forward = this->qOrientation * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = this->qOrientation * glm::vec3(0.0f, 1.0f, 0.0f);


	// TODO: redo all this
	this->acceleration = glm::vec3(0.0f);
	if (this->forwardAccel == 0.0f && (this->velocity.length() != 0.0f))
	{
		// decel fwd
		this->acceleration -= glm::normalize(this->velocity) * this->forwardDecel;
		if ((fabs(this->velocity.x) + fabs(this->velocity.z)) <= 0.1f)
		{
			this->velocity.x = 0.0f;
			this->velocity.y = 0.0f;
			this->velocity.z = 0.0f;
			this->acceleration.x = 0.0f;
			this->acceleration.y = 0.0f;
			this->acceleration.z = 0.0f;
		}
	}
	else
		this->acceleration += forward * this->forwardAccel;

	//if (this->updownAccel == 0.0f && this->velocity.y != 0.0f)
	//{
	//	// decel up/down
	//	this->acceleration -= glm::normalize(glm::vec3(0.0f, this->velocity.y, 0.0f)) * this->updownDecel;
	//	if (fabs(this->velocity.y) <= 0.1f)
	//	{
	//		this->velocity.y = 0.0f;
	//		this->acceleration.y = 0.0f;
	//	}
	//}
	//else
	//	this->acceleration += up * this->updownAccel;

	this->speed = length(glm::vec3(this->velocity.x, this->velocity.y, this->velocity.z));
	if (this->speed > maxSpeed)
		this->velocity = glm::normalize(glm::vec3(this->velocity.x, this->velocity.y, this->velocity.z)) * maxSpeed;

	//float updownspeed = fabs(this->velocity.y);
	//if (updownspeed > this->maxUpDownSpeed)
	//	this->velocity.y = copysignf(1.0, this->velocity.y) * this->maxUpDownSpeed;
	
	if (this->YrotationSpeed != 0.0f && leftrightAccel == 0.0f)
	{
		// decel rotation
		this->YrotationSpeed += copysignf(1.0f, this->YrotationSpeed) * -1.0f * leftrightDecel * dt;
		if (fabs(this->YrotationSpeed) <= 0.1f)
			this->YrotationSpeed = 0.0f;
	}
	else
		this->YrotationSpeed += leftrightAccel * dt;

	if (fabs(this->YrotationSpeed) > maxRotationSpeed)
		this->YrotationSpeed = copysignf(1.0f, this->YrotationSpeed) * maxRotationSpeed;

	if (this->XrotationSpeed != 0.0f && updownAccel == 0.0f)
	{
		// decel rotation
		this->XrotationSpeed += copysignf(1.0f, this->XrotationSpeed) * -1.0f * updownDecel * dt;
		if (fabs(this->XrotationSpeed) <= 0.1f)
			this->XrotationSpeed = 0.0f;
	}
	else
		this->XrotationSpeed += updownAccel * dt;

	if (fabs(this->XrotationSpeed) > maxRotationSpeed)
		this->XrotationSpeed = copysignf(1.0f, this->XrotationSpeed) * maxRotationSpeed;

	if (this->ZrotationSpeed != 0.0f && rollAccel == 0.0f)
	{
		// decel rotation
		this->ZrotationSpeed += copysignf(1.0f, this->ZrotationSpeed) * -1.0f * rollDecel * dt;
		if (fabs(this->ZrotationSpeed) <= 0.1f)
			this->ZrotationSpeed = 0.0f;
	}
	else
		this->ZrotationSpeed += rollAccel * dt;

	if (fabs(this->ZrotationSpeed) > maxRotationSpeed)
		this->ZrotationSpeed = copysignf(1.0f, this->ZrotationSpeed) * maxRotationSpeed;

	this->rotate(glm::vec3(XrotationSpeed, YrotationSpeed, ZrotationSpeed) * dt);
}

sMessage cPelican::message(sMessage const& msg)
{
	return sMessage();
}

void cPelican::instatiateUniqueVariables(Json::Value& obj)
{
}

void cPelican::serializeUniqueVariables(Json::Value& obj)
{
}
