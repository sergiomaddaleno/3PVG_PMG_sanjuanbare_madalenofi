#ifndef SCR
#define SCR

#include <lua.hpp>

#include <memory>
#include <string>
#include <iterator>
#include <iostream>
#include <fstream>
#include <filesystem>

class LuaScript {

public:
	LuaScript()
		: state_{ luaL_newstate(),&lua_close }
		, s{ state_.get() } {
		luaL_openlibs(s);
	}

	void check(int error) {
		if (error != LUA_OK) {
			std::string err = lua_tostring(s, lua_gettop(s));
			lua_pop(s, lua_gettop(s));
			throw std::runtime_error("Lua error: " + err);
		}
	}

	void run(const std::string& str) {
		check(luaL_loadstring(s, str.c_str()));
		check(lua_pcall(s, 0, 0, 0));
		lua_pop(s, lua_gettop(s));
	}

	void add_global(int(*function)(lua_State*), const std::string& name) {
		lua_pushcfunction(s, function); 
		lua_setglobal(s, name.c_str());
	}

private:
	std::unique_ptr<lua_State, decltype(&lua_close)> state_;
	lua_State* s;
};

int multiply(lua_State* s) {
	lua_Integer a = luaL_checkinteger(s, 1);
	lua_Integer b = luaL_checkinteger(s, 2);
	lua_Integer c = a * b;
	lua_pushinteger(s, c);
	return 1; // Devolver el numero de parametros que devuelve la funcion, en este caso 1
}

#endif // SCR


