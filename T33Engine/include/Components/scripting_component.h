#ifndef __SCRIPTING_COMPONENT__
#define __SCRIPTING_COMPONENT__

#include <string>
#include <memory>

#include "../deps/nlohmann/json.hpp"

#include "Systems/scripting.h"
#include "Render/scripting_system.h"

struct ScriptingComponent {
    std::string script_path;
    int entity_id;

    nlohmann::json serialize() const;

    ScriptingComponent();
    ~ScriptingComponent();
};


#endif // __SCRIPTING_COMPONENT__
