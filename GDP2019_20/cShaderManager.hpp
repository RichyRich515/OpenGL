#ifndef _cShaderManager_HG_
#define _cShaderManager_HG_

#include <string>
#include <vector>
#include <map>
#include "GLCommon.h"

class cShaderManager
{
public:
	class cShader
	{
	public:
		cShader();
		~cShader();

		enum eShaderType
		{
			VERTEX_SHADER,
			FRAGMENT_SHADER,
			UNKNOWN
		};

		eShaderType shaderType;
		std::string getShaderTypeString();

		unsigned int ID;	// or "name" from OpenGL
		std::vector<std::string> vecSource;
		bool bSourceIsMultiLine;
		std::string fileName;

	};

	class cShaderProgram
	{
	public:
		cShaderProgram() : ID(0) {};
		~cShaderProgram() {};

		unsigned int ID; // ID from OpenGL (calls it a "name")
		std::string friendlyName; // We give it this name

		std::map<std::string, int> mapUniformName_to_UniformLocation;

		// Look up the uniform location and save it.
		bool LoadUniformLocation(std::string variableName);

		// Loads all active uniforms to the map
		void LoadActiveUniforms();

		int getUniformLocID(std::string uniformname);
	};

	cShaderManager();
	~cShaderManager();

	bool useShaderProgram(unsigned int ID);
	bool useShaderProgram(std::string friendlyName);
	bool createProgramFromFile(std::string friendlyName, cShader& vertexShad, cShader& fragShader);
	void setBasePath(std::string basepath);
	unsigned int getIDFromFriendlyName(std::string friendlyName);

	// Used to load the uniforms. Returns NULL if not found.
	cShaderProgram* pGetShaderProgramFromFriendlyName(std::string friendlyName);

	// Clears last error
	std::string getLastError();


	static void setCurrentShader(cShaderProgram* s) { current_shader = s; }
	static cShaderProgram* getCurrentShader() { return current_shader; }
private:

	static cShaderProgram* current_shader;

	// Returns an empty string if it didn't work
	bool m_loadSourceFromFile(cShader& shader);
	std::string m_basepath;

	bool m_compileShaderFromSource(cShader& shader, std::string& error);
	// returns false if no error
	bool m_wasThereACompileError(unsigned int shaderID, std::string& errorText);
	bool m_wasThereALinkError(unsigned int progID, std::string& errorText);

	std::string m_lastError;

	std::map<unsigned int, cShaderProgram> m_ID_to_Shader;
	std::map<std::string, unsigned int> m_name_to_ID;
};

#endif