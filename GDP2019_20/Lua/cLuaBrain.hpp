#pragma once

extern "C" {
#include <Lua5.3.5/lua.h>
#include <Lua5.3.5/lauxlib.h>
#include <Lua5.3.5/lualib.h>
}

#include <string>
#include <vector>
//#include "cGameObject.h"
#include <map>


// Forward declaration for cyclical reference
class cGameObject;

class cLuaBrain
{
public:
	// Init Lua and set callback functions
	cLuaBrain();
	~cLuaBrain();

	void setScriptPath(std::string const& path);

	void loadScript(std::string const& scriptName, cGameObject* go);

	// Call all the active scripts that are loaded
	void Update(float deltaTime, float totalTime);

	// - Name (sring)
	static int l_GetObjectByName(lua_State* L);

	// Called by Lua
	// Passes object ID, new velocity, etc.
	// Returns valid (true or false)
	// Passes: 
	// - position (xyz)
	// - velocity (xyz)
	// called "setObjectState" in lua
	static int l_SetObjectPosition(lua_State* L);

	// Returns true if success
	// - object ID
	// - position (xyz)
	// - velocity (xyz)
	// called "getObjectState" in lua
	static int l_GetObjectPosition(lua_State* L);

	// object ID
	// command ID
	// returns true if success
	// "setObjectCommand" in lua
	static int l_SetObjectCommand(lua_State* L);

	// "parent" command ID
	// command ID
	// returns true if success
	// "setParallelCommand" in lua
	static int l_SetParallelCommand(lua_State* L);

	// object ID
	// duration
	// destination
	// returns command ID
	// "createCommand_MoveToTimed" in lua
	static int l_CreateCommand_MoveToTimed(lua_State* L);

	// object ID
	// duration
	// end rotation XYZ degrees
	// returns command ID
	// "createCommand_MoveToTimed" in lua
	static int l_CreateCommand_RotateToTimed(lua_State* L);

	// - Start (X, Y, Z)
	// - End (X, Y, Z)
	// - Color (R, G, B)
	// - Lifetime (f)
	static int l_DrawDebugLine(lua_State* L);

	// - P1 (X, Y, Z)
	// - P2 (X, Y, Z)
	// - P3 (X, Y, Z)
	// - Color (R, G, B)
	// - Lifetime (f)
	static int l_DrawDebugTri(lua_State* L);


private:
	std::string m_script_path;

	std::string script_name;
	std::string script;

	cGameObject* go;

	lua_State* m_pLuaState;

	std::string m_decodeLuaErrorToString(int error);
};