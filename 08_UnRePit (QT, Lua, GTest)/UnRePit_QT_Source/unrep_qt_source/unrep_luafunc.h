#pragma once
#ifndef UNREP_LUAFUNC_H
#define UNREP_LUAFUNC_H

extern "C" {
#include "LuaLibs/lua.hpp"
}

#include <string>

namespace {
	static const char* LuaSettingsFile = "UnRePit_Settings.lua";
	static const char* LuaReformerFile = "UnRePit_LuaReformer.lua";
}

int GetLuaInt(const char* IntName) noexcept {
	int LuaInt{};

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_dofile(L, LuaSettingsFile) == LUA_OK) {
		// Retrieve integer variable from Lua environment
		lua_getglobal(L, IntName);
		if (lua_isnumber(L, -1)) {
			LuaInt = static_cast<int>(lua_tointeger(L, -1));
		}
		lua_pop(L, 1);  // Pop integer from the stack
	}

	lua_close(L);
	return LuaInt;
}

bool GetLuaBool(const char* BoolName) noexcept {
	bool LuaBool{ false };

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_dofile(L, LuaSettingsFile) == LUA_OK) {
		// Retrieve boolean from Lua environment
		lua_getglobal(L, BoolName);
		if (lua_isboolean(L, -1)) {
			LuaBool = static_cast<bool>(lua_toboolean(L, -1));
		}
		lua_pop(L, 1);  // Pop boolean from the stack
	}

	lua_close(L);
	return LuaBool;
}

bool CallLuaReformer(std::string& Input) noexcept {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	// Load and execute Lua script from a file
	if (luaL_loadfile(L, LuaReformerFile) || lua_pcall(L, 0, 0, 0)) {
        // const char* errorMsg = lua_tostring(L, -1);
		// Handle unexpected error
	}

	// Get the Lua function onto the stack
	lua_getglobal(L, "ReformText");

	// Push arguments onto the stack
	lua_pushstring(L, Input.c_str());

	// Call the function with error protection
	int status = lua_pcall(L, 1, 1, 0);
	if (status == LUA_OK) {
		// Function executed successfully, retrieve result
		if (lua_isstring(L, -1)) {
			Input = lua_tostring(L, -1);
		}
		else {
			Input.clear();
			Input.append("Lua error: ");
			Input.append(lua_tostring(L, -1));
			return true;
		}
	}
	else {
		Input.clear();
		Input.append("Lua error: ");
		Input.append(lua_tostring(L, -1));
		return true;
	}

	// Close Lua state
	lua_close(L);

	return false;
}

/////////////////////////////////////////////////////////////////////////////

class LuaInterpreter {
private:
	static std::string LuaOutput;  // This will store the captured output
	LuaInterpreter() = delete;
	~LuaInterpreter() = delete;

	// C++ function that replaces Lua's print
	static int CustomPrint(lua_State* L) noexcept {
		LuaInterpreter* interpreter = reinterpret_cast<LuaInterpreter*>(lua_touserdata(L, lua_upvalueindex(1)));

		int nargs = lua_gettop(L);

		for (int i = 1; i <= nargs; ++i) {
			if (lua_isnil(L, i)) {
				interpreter->LuaOutput.append("nil");
			}
			else if (lua_isboolean(L, i)) {
				interpreter->LuaOutput.append(lua_toboolean(L, i) ? "true" : "false");
			}
			else if (lua_isnumber(L, i)) {
				double num = lua_tonumber(L, i);
				if (num == static_cast<int>(num)) {
					interpreter->LuaOutput.append(std::to_string(static_cast<int>(num)));
				}
				else {
					interpreter->LuaOutput.append(std::to_string(num));
				}
			}
			else if (lua_isstring(L, i)) {
				interpreter->LuaOutput.append(lua_tostring(L, i));
			}
			else if (lua_istable(L, i)) {
				interpreter->LuaOutput.append("table: ");
				interpreter->LuaOutput.append(std::to_string(reinterpret_cast<intptr_t>(lua_topointer(L, i))));
			}
			else if (lua_isfunction(L, i)) {
				interpreter->LuaOutput.append("function: ");
				interpreter->LuaOutput.append(std::to_string(reinterpret_cast<intptr_t>(lua_topointer(L, i))));
			}
			else if (lua_isuserdata(L, i)) {
				interpreter->LuaOutput.append("userdata: ");
				interpreter->LuaOutput.append(std::to_string(reinterpret_cast<intptr_t>(lua_touserdata(L, i))));
			}
			else if (lua_isthread(L, i)) {
				interpreter->LuaOutput.append("thread: ");
				interpreter->LuaOutput.append(std::to_string(reinterpret_cast<intptr_t>(lua_tothread(L, i))));
			}
			if (i < nargs) {
				interpreter->LuaOutput.push_back('\t');
			}
		}

		interpreter->LuaOutput.push_back('\r');
		interpreter->LuaOutput.push_back('\n');

		return 0;
	}
public:

	static inline void FlushOutput() noexcept {
		LuaOutput.clear();
	}

	static std::string ReadLua(const char* LuaCode) noexcept {
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);

		// Register the custom print function
		lua_pushcclosure(L, CustomPrint, 1);
		lua_setglobal(L, "print");

		// Execute Lua code
		if (luaL_dostring(L, LuaCode) != LUA_OK) {
			std::string ErrorMsg{"Lua error: "};
			ErrorMsg.append(lua_tostring(L, -1));
			return ErrorMsg;
		}

		// Clean up Lua state
		lua_close(L);

		return LuaOutput;
	}
};

std::string LuaInterpreter::LuaOutput{};

#endif // UNREP_LUAFUNC_H
