/**
 * @file scripting.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SCRIPTING__
#define __SCRIPTING__

#include <lua.hpp>

#include <memory>
#include <string>
#include <iterator>
#include <iostream>
#include <fstream>
#include <filesystem>

 /**
  * @class LuaScript
  * @brief Manages Lua scripting functionality.
  */
class LuaScript {

public:
	LuaScript();
	~LuaScript();

	void check(int error);
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

#endif // __SCRIPTING__


