
#include "Render/scripting_system.h"
#include "render/physics_system.h"
#include "Components/components.h"

#include "Systems/ecs_manager.h"

#include "utils.h"

ScriptingSystem::ScriptingSystem(ECSManager& ecs) :
	ecs_(ecs)
{
}

ScriptingSystem::~ScriptingSystem()
{
}

void ScriptingSystem::init() {
    auto si = ecs_.getComponentList<ScriptingComponent>().begin();
    auto siend = ecs_.getComponentList<ScriptingComponent>().end();

    for (; si != siend; ++si) {
        if (si->has_value()) {
            auto& script_comp = *si;

            loadScript(*script_comp);
        }
    }
}

void ScriptingSystem::loadScript(ScriptingComponent& script_comp) {
    char* loc_lua_file = ReadFile(script_comp.script_path.c_str());

    // Create a new Lua instance for this entity if it doesn't exist
    LuaScript& lua_script = entity_scripts_[script_comp.entity_id];

    lua_State* L = lua_script.get_state();
    lua_pushinteger(L, script_comp.entity_id);
    lua_setglobal(L, "CURRENT_ENTITY");

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, script_comp.entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->get_transform_component(L, entity);
        }, 2);
    lua_setglobal(L, "getTransform");

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, script_comp.entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->set_transform_component(L, entity);
        }, 2);
    lua_setglobal(L, "setTransform");

    lua_script.run(loc_lua_file);
}


void ScriptingSystem::update(float dt) {
    for (auto& [entity, lua_script] : entity_scripts_) {
        lua_script.call_function("update", dt);
    }
}

int ScriptingSystem::get_transform_component(lua_State* lua_state, size_t entity) {
    
    if (ecs_.has_component<TransformComponent>(entity)) {
        auto& transf = ecs_.get_component<TransformComponent>(entity);
        lua_pushnumber(lua_state, transf->position.x);
        lua_pushnumber(lua_state, transf->position.y);
        lua_pushnumber(lua_state, transf->position.z);
        return 3;
    }
    luaL_error(lua_state, "Entity %d missing TransformComponent", entity);
    return 0;
}


int ScriptingSystem::set_transform_component(lua_State* lua_state, size_t entity) {
    glm::vec3 position;
    position.x = static_cast<float>(luaL_checknumber(lua_state, 1));
    position.y = static_cast<float>(luaL_checknumber(lua_state, 2));
    position.z = static_cast<float>(luaL_checknumber(lua_state, 3));

    if (ecs_.has_component<TransformComponent>(entity)) {
        auto& transf = ecs_.get_component<TransformComponent>(entity);
        transf->position = position;
        return 0;
    }
    luaL_error(lua_state, "Entity %d missing TransformComponent", entity);
    return 0;
}

void ScriptingSystem::registerTransform(lua_State* L, size_t entity_id) {
    // Register getTransform function
    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->get_transform_component(L, entity);
        }, 2);
    lua_setglobal(L, "getTransform");

    // Register setTransform function
    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->set_transform_component(L, entity);
        }, 2);
    lua_setglobal(L, "setTransform");
}
