#include "cLight.hpp"

#include <stdio.h>

cLight::cLight()
{
	this->param2.x = 0; // Disabled
}

cLight::cLight(unsigned i, Json::Value obj, GLuint program)
{
	char buffer[MAX_BUFFER];
	sprintf_s(buffer, "lights[%d].%s", i, "position");
	this->position_loc = glGetUniformLocation(program, buffer);
	sprintf_s(buffer, "lights[%d].%s", i, "diffuse");
	this->diffuse_loc =	glGetUniformLocation(program, buffer);
	sprintf_s(buffer, "lights[%d].%s", i, "specular");
	this->specular_loc = glGetUniformLocation(program, buffer);
	sprintf_s(buffer, "lights[%d].%s", i, "atten");
	this->atten_loc = glGetUniformLocation(program, buffer);
	sprintf_s(buffer, "lights[%d].%s", i, "direction");
	this->direction_loc = glGetUniformLocation(program, buffer);
	sprintf_s(buffer, "lights[%d].%s", i, "param1");
	this->param1_loc = glGetUniformLocation(program, buffer);
	sprintf_s(buffer, "lights[%d].%s", i, "param2");
	this->param2_loc = glGetUniformLocation(program, buffer);

	for (unsigned j = 0; j < 4; ++j) // length of vec4
	{
		this->position[j] = obj["position"][j].asFloat();
		this->diffuse[j] = obj["diffuse"][j].asFloat();;
		this->specular[j] = obj["specular"][j].asFloat();
		this->atten[j] = obj["atten"][j].asFloat();
		this->direction[j] = obj["direction"][j].asFloat();
		this->param1[j] = obj["param1"][j].asFloat();
		this->param2[j] = obj["param2"][j].asFloat();
	}

	// TODO: check these 1.0fs
	glUniform4f(this->position_loc, this->position.x, this->position.y, this->position.z, this->position.w);
	glUniform4f(this->diffuse_loc, this->diffuse.r, this->diffuse.g, this->diffuse.b, this->diffuse.a);
	glUniform4f(this->specular_loc, this->specular.r, this->specular.g, this->specular.b, this->specular.a);
	glUniform4f(this->atten_loc, this->atten.x, this->atten.y, this->atten.z, this->atten.w);
	glUniform4f(this->direction_loc, this->direction.x, this->direction.y, this->direction.z, this->direction.w);
	glUniform4f(this->param1_loc, this->param1.x, this->param1.y, this->param1.z, this->param1.w);
	glUniform4f(this->param2_loc, this->param2.x, this->param2.y, this->param2.z, this->param2.w);
}

