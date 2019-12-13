#include "cLuaBrain.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "../DebugRenderer/cDebugRenderer.h"
#include "../cWorld.hpp"
#include "../cGameObject.hpp"
#include "../cCommand.hpp"


cLuaBrain::cLuaBrain()
{
	this->go = nullptr;

	// Create new Lua state.
	// NOTE: this is common to ALL script in this case
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState); /* Lua 5.3.3 */

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetObjectID);
	lua_setglobal(this->m_pLuaState, "getObjectID");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_SetObjectPosition);
	lua_setglobal(this->m_pLuaState, "setObjectPosition");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetObjectPosition);
	lua_setglobal(this->m_pLuaState, "getObjectPosition");


	// Command stuff
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_SetObjectCommand);
	lua_setglobal(this->m_pLuaState, "setObjectCommand");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddParallelCommand);
	lua_setglobal(this->m_pLuaState, "addParallelCommand");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddSerialCommand);
	lua_setglobal(this->m_pLuaState, "addSerialCommand");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_CreateCommand_SetVisible);
	lua_setglobal(this->m_pLuaState, "createCommand_SetVisible");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_CreateCommand_SetPosition);
	lua_setglobal(this->m_pLuaState, "createCommand_SetPosition");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_CreateCommand_Wait);
	lua_setglobal(this->m_pLuaState, "createCommand_Wait");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_CreateCommand_MoveToTimed);
	lua_setglobal(this->m_pLuaState, "createCommand_MoveToTimed");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_CreateCommand_RotateToTimed);
	lua_setglobal(this->m_pLuaState, "createCommand_RotateToTimed");


	// Debug rendering
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_DrawDebugLine);
	lua_setglobal(this->m_pLuaState, "drawDebugLine");
	
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_DrawDebugTri);
	lua_setglobal(this->m_pLuaState, "drawDebugTri");

	return;
}

cLuaBrain::~cLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}

void cLuaBrain::setScriptPath(std::string const& path)
{
	this->m_script_path = path;
}


// Saves (and overwrites) any script
// scriptName is just so we can delete them later
void cLuaBrain::loadScript(std::string const& scriptName, cGameObject* go)
{
	std::ifstream scriptFile;
	this->script_name = scriptName;
	this->script = "";
	this->go = go;
	// TODO: non-hard coded script path
	scriptFile.open("assets\\LuaScripts\\" + this->script_name);
	for (std::string line; std::getline(scriptFile, line);)
	{
		this->script += line + "\n";
	}
	return;
}


// Call all the active scripts that are loaded
void cLuaBrain::Update(float deltaTime, float totalTime)
{
	// TODO: Wow.... .c_str() every frame??
	std::ostringstream oss;
	oss << "dt = " << deltaTime << "\n"
		<< "tt = " << totalTime << "\n"
		<< "id = " << (long long)go << "\n"
		<< script;

	int error = luaL_loadstring(this->m_pLuaState, oss.str().c_str());
	if (error)
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: " << this->script_name << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->m_pLuaState,	/* lua state */
		0,	/* nargs: number of arguments pushed onto the lua stack */
		0,	/* nresults: number of results that should be on stack at end*/
		0);	/* errfunc: location, in stack, of error function.
	
	if 0, results are on top of stack. */
	if (error)
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(this->m_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: " << this->script_name << std::endl;
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
		return;
	}
}

int cLuaBrain::l_GetObjectID(lua_State* L)
{
	std::string name = lua_tostring(L, 1);

	cWorld* world = cWorld::getWorld();
	std::vector<cGameObject*>::iterator itr = std::find_if(world->vecGameObjects.begin(), world->vecGameObjects.end(), 
		[name](cGameObject* o) { return name == o->name; });

	if (itr != world->vecGameObjects.end())
	{
		lua_pushnumber(L, (long long)(*itr));
	}
	else
	{
		lua_pushnumber(L, 0);
	}
	return 1;
}

int cLuaBrain::l_SetObjectPosition(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid
	// Get the values that lua pushed and update object
	go->position.x = (float)lua_tonumber(L, 2);
	go->position.y = (float)lua_tonumber(L, 3);
	go->position.z = (float)lua_tonumber(L, 4);

	lua_pushboolean(L, true); // index is OK

	return 1; // There was 1 thing on the stack
}


int cLuaBrain::l_GetObjectPosition(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	// Object ID is valid
	lua_pushboolean(L, true);
	lua_pushnumber(L, go->position.x);
	lua_pushnumber(L, go->position.y);
	lua_pushnumber(L, go->position.z);

	return 4; // There were 7 things on the stack
}


int cLuaBrain::l_SetObjectCommand(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	go->command = (iCommand*)(long long)(lua_tonumber(L, 2));

	// Object ID is valid
	lua_pushboolean(L, true);
	return 1;
}

int cLuaBrain::l_AddParallelCommand(lua_State* L)
{
	iCommand* pcmd = (iCommand*)(long long)(lua_tonumber(L, 1));
	if (pcmd == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}
	iCommand* ccmd = (iCommand*)(long long)(lua_tonumber(L, 2));
	if (ccmd == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	pcmd->addParallel(ccmd);
	

	// valid
	lua_pushboolean(L, true);
	return 1;
}

int cLuaBrain::l_AddSerialCommand(lua_State* L)
{
	iCommand* pcmd = (iCommand*)(long long)(lua_tonumber(L, 1));
	if (pcmd == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}
	iCommand* ccmd = (iCommand*)(long long)(lua_tonumber(L, 2));
	if (ccmd == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	pcmd->addSerial(ccmd);


	// valid
	lua_pushboolean(L, true);
	return 1;
}

int cLuaBrain::l_CreateCommand_SetPosition(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	cCommand_SetPosition* command = new cCommand_SetPosition(go,
		glm::vec3(		// destination
			(float)lua_tonumber(L, 2),
			(float)lua_tonumber(L, 3),
			(float)lua_tonumber(L, 4)
		));
	lua_pushnumber(L, (long long)command);
	return 1;
}

int cLuaBrain::l_CreateCommand_SetVisible(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));
	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	cCommand_SetVisible* command = new cCommand_SetVisible(go, lua_toboolean(L, 2));
	lua_pushnumber(L, (long long)command);
	return 1;
}

int cLuaBrain::l_CreateCommand_Wait(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	cCommand_Wait* command = new cCommand_Wait(go, (float)lua_tonumber(L, 2));
	lua_pushnumber(L, (long long)command);
	return 1; // There was 1 thing on the stack
}


int cLuaBrain::l_CreateCommand_MoveToTimed(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	// Object ID is valid
	
	cCommand_MoveToTimed* command = new cCommand_MoveToTimed(
		go, // game object
		(float)lua_tonumber(L, 2), // duration
		glm::vec3(		// destination
			(float)lua_tonumber(L, 3), 
			(float)lua_tonumber(L, 4), 
			(float)lua_tonumber(L, 5)
		)
	);

	lua_pushnumber(L, (long long)command);

	return 1; // There was 1 thing on the stack
}


int cLuaBrain::l_CreateCommand_RotateToTimed(lua_State* L)
{
	cGameObject* go = (cGameObject*)(long long)(lua_tonumber(L, 1));

	if (go == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	// Object ID is valid

	cCommand_RotateToTimed* command = new cCommand_RotateToTimed(
		go, // game object
		(float)lua_tonumber(L, 2), // duration
		glm::vec3(		// destination
			glm::radians((float)lua_tonumber(L, 3)),
			glm::radians((float)lua_tonumber(L, 4)),
			glm::radians((float)lua_tonumber(L, 5))
		)
	);

	lua_pushnumber(L, (long long)command);

	return 1; // There was 1 thing on the stack
}

int cLuaBrain::l_DrawDebugLine(lua_State* L)
{
	if (cWorld::debugMode)
	{
		cWorld::pDebugRenderer->addLine(
			glm::vec3((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3)),
			glm::vec3((float)lua_tonumber(L, 4), (float)lua_tonumber(L, 5), (float)lua_tonumber(L, 6)),
			glm::vec3((float)lua_tonumber(L, 7), (float)lua_tonumber(L, 8), (float)lua_tonumber(L, 9)),
			(float)lua_tonumber(L, 10));
	}

	return 0;
}


int cLuaBrain::l_DrawDebugTri(lua_State* L)
{
	if (cWorld::debugMode)
	{
		cWorld::pDebugRenderer->addTriangle(
			glm::vec3((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3)),
			glm::vec3((float)lua_tonumber(L, 4), (float)lua_tonumber(L, 5), (float)lua_tonumber(L, 6)),
			glm::vec3((float)lua_tonumber(L, 7), (float)lua_tonumber(L, 8), (float)lua_tonumber(L, 9)),
			glm::vec3((float)lua_tonumber(L, 10), (float)lua_tonumber(L, 11), (float)lua_tonumber(L, 12)),
			(float)lua_tonumber(L, 13));
	}

	return 0;
}


std::string cLuaBrain::m_decodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
	case LUA_ERRRUN:
		return "Lua: Runtime error";
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
	default:
		// Who knows what this error is?
		return "Lua: UNKNOWN error";
	} //switch ( error )
}
