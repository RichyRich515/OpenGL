// cModelLoader.hpp
// Basic model loader class
// Richard Woods
// 2019-09-09

#pragma once

#include <string>
#include "cMesh.hpp"

class cModelLoader
{
public:
	cModelLoader();
	~cModelLoader();

	/*
		Input:
			std::string filename - File to load
		Output:
			cMesh &mesh - Mesh by reference
	*/
	bool loadModel(std::string filename, cMesh* mesh);
};