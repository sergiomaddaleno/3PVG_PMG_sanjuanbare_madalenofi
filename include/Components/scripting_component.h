#ifndef SRC
#define SRC

#include <memory>
#include "scripting.h"

struct ScriptingComponent {

	std::shared_ptr<LuaScript> lua_script = nullptr;

	ScriptingComponent(){}
	~ScriptingComponent(){}
};

#endif
