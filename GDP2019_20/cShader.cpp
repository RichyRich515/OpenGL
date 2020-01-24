#include "cShaderManager.hpp"
#include "GLCommon.h"		// glfw.h, etc. (openGL.h)

cShaderManager::cShader::cShader()
{
	this->ID = 0;
	this->shaderType = cShader::UNKNOWN;
	return;
}

cShaderManager::cShader::~cShader()
{
	return;
}

std::string cShaderManager::cShader::getShaderTypeString(void)
{
	switch (this->shaderType)
	{
	case cShader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case cShader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case cShader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}

// Look up the uniform inside the shader, then save it, if it finds it
bool cShaderManager::cShaderProgram::LoadUniformLocation(std::string variableName)
{
	GLint uniLocation = glGetUniformLocation(this->ID, variableName.c_str());
	if (uniLocation == -1)
		return false;
	this->mapUniformName_to_UniformLocation[variableName.c_str()] = uniLocation;
	return true;
}

void cShaderManager::cShaderProgram::LoadActiveUniforms()
{
	int maxNameSize = 0;
	glGetProgramiv(this->ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameSize);


	int uniformCount = 0;
	glGetProgramiv(this->ID, GL_ACTIVE_UNIFORMS, &uniformCount);

	char* uniformName = new char[maxNameSize];
	for (int i = 0; i < uniformCount; ++i)
	{
		GLsizei numCharWritten = 0;
		GLint sizeOfVariable = 0;
		GLenum uniformType = 0;

		memset(uniformName, 0, maxNameSize);

		glGetActiveUniform(this->ID, i, maxNameSize, &numCharWritten, &sizeOfVariable, &uniformType, uniformName);

		this->mapUniformName_to_UniformLocation[uniformName] = glGetUniformLocation(this->ID, uniformName);
	}

	delete[] uniformName;
}

GLint cShaderManager::cShaderProgram::getUniformLocID(std::string uniformname)
{
	//auto itUniform = this->mapUniformName_to_UniformLocation.find(uniformname);
	//if (itUniform == this->mapUniformName_to_UniformLocation.end())
	//	return -1; // Not found
	//return itUniform->second;
	return this->mapUniformName_to_UniformLocation[uniformname];
}