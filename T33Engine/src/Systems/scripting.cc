
#include "Systems/scripting.h"

LuaScript::LuaScript() 
	: state_{ luaL_newstate(),&lua_close }
	, s{ state_.get() } 

{ luaL_openlibs(s);}

LuaScript::~LuaScript()
{
}

void LuaScript::check(int error)
{
	if (error != LUA_OK) {
		std::string err = lua_tostring(s, lua_gettop(s));
		lua_pop(s, lua_gettop(s));
		throw std::runtime_error("Lua error: " + err);
	}
}

void LuaScript::run(const std::string& str) {
	check(luaL_loadstring(s, str.c_str()));
	check(lua_pcall(s, 0, 0, 0));
	lua_pop(s, lua_gettop(s));
}

void LuaScript::add_global(int(*function)(lua_State*), const std::string& name) {
	lua_pushcfunction(s, function);
	lua_setglobal(s, name.c_str());
}

// ---

void LuaScript::get_table(const std::string& table_name) {
	lua_getglobal(s, table_name.c_str());
	if (!lua_istable(s, -1)) {
		lua_pop(s, 1);
	}
}

int LuaScript::get_integer(const std::string& var_name) {
	if (lua_istable(s, -1)) {
		lua_getfield(s, -1, var_name.c_str());
	}
	else {
		lua_getglobal(s, var_name.c_str());
	}

	int value = (int)lua_tointeger(s, -1);
	lua_pop(s, 1);
	return value;
}

float LuaScript::get_float(const std::string& var_name) {
	if (lua_istable(s, -1)) {
		lua_getfield(s, -1, var_name.c_str());
	}
	else {
		lua_getglobal(s, var_name.c_str());
	}

	float value = (float)lua_tonumber(s, -1);
	lua_pop(s, 1);
	return value;
}

bool LuaScript::get_bool(const std::string& var_name) {
	if (lua_istable(s, -1)) {
		lua_getfield(s, -1, var_name.c_str());
	}
	else {
		lua_getglobal(s, var_name.c_str());
	}

	bool value = lua_toboolean(s, -1);
	lua_pop(s, 1);
	return value;
}

// ---

int multiply(lua_State* s) {
	lua_Integer a = luaL_checkinteger(s, 1);
	lua_Integer b = luaL_checkinteger(s, 2);
	lua_Integer c = a * b;
	lua_pushinteger(s, c);
	return 1; // Devolver el numero de parametros que devuelve la funcion, en este caso 1
}