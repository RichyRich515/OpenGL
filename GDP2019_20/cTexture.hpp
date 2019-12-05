#pragma once

#include <string>

// 0 to 6 textures
// 10 cube map for skybox
// 15 height map per gameobject

class cTexture
{
public:
	std::string fileName;
	float xOffset;
	float yOffset;
	float blend;
	float tiling;
};