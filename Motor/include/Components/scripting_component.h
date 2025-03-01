#ifndef __SCRIPTING_COMPONENT__
#define __SCRIPTING_COMPONENT__

#include <memory>
#include "Systems/scripting.h"

struct ScriptingComponent {

	std::shared_ptr<LuaScript> lua_script;

	ScriptingComponent();
	~ScriptingComponent();
};

#endif // __SCRIPTING_COMPONENT__
