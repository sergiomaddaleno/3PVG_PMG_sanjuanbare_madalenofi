#include "Components/scripting_component.h"

ScriptingComponent::ScriptingComponent()
	: lua_script(std::make_shared<LuaScript>()) {}

ScriptingComponent::~ScriptingComponent(){}
