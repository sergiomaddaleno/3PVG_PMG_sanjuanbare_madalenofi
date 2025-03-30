/**
 * @file scripting_system.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SCRIPTING_SYSTEM__
#define __SCRIPTING_SYSTEM__

#include "../Systems/ecs_manager.h"
#include "Systems/scripting.h"
#include "Core/shader.h"

struct ScriptingComponent;

class ScriptingSystem {
public:
    ScriptingSystem(ECSManager& ecs);
    ~ScriptingSystem();

    void init();

    void loadScript(ScriptingComponent& script_comp);
    void update(float dt);

    int get_transform_component(lua_State* lua_state, size_t entity);
    int set_transform_component(lua_State* lua_state, size_t entity);

private:

    void registerTransform(lua_State* L, size_t entity_id);

    std::unordered_map<size_t, LuaScript> entity_scripts_;
    ECSManager& ecs_;
};

#endif // !__SCRIPTING_SYSTEM__
