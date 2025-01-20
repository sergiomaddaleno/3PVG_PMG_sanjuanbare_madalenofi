#ifndef SRC
#define SRC

#include <memory>
#include "Systems/scripting.h"

struct ScriptingComponent {

	std::shared_ptr<LuaScript> lua_script = nullptr;

	ScriptingComponent() : lua_script(std::make_shared<LuaScript>()) {}
	~ScriptingComponent(){}
};

#endif
