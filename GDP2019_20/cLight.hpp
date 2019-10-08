#pragma once

#include "GLCommon.h"

#include <json/json.h>

#include <glm/glm.hpp>

constexpr unsigned MAX_BUFFER = 256;

class cLight
{
public:
	cLight();
	cLight(unsigned i, Json::Value obj, GLuint program);
	~cLight() {}

	Json::Value serializeJSONObject();

	void updateShaderUniforms();

	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;		// rgb = highlight colour, w = power 1 - 10000
	glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 direction;	// Spot, directional lights

	glm::vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
						// 0 = pointlight
						// 1 = spot light
						// 2 = directional light

	glm::vec4 param2;	// x = 0 for off, 1 for on


	GLuint position_loc;
	GLuint diffuse_loc;
	GLuint specular_loc;
	GLuint atten_loc;
	GLuint direction_loc;
	GLuint param1_loc;
	GLuint param2_loc;
};