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

	void run(const std::string& str);

	void add_global(int(*function)(lua_State*), const std::string& name);
	
	// --

	void get_table(const std::string& table_name);

	int get_integer(const std::string& var_name);

	float get_float(const std::string& var_name);

	bool get_bool(const std::string& var_name);

	// --

	int stack_size() { return lua_gettop(s); }

	void pop(int count = 1) { lua_pop(s, count); }

	lua_State* get_state() { return s; }

private:
	std::unique_ptr<lua_State, decltype(&lua_close)> state_;
	lua_State* s;
};

int multiply(lua_State* s);

#endif // SCR


