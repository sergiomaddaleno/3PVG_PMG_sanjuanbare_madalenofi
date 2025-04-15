
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

void ScriptingSystem::init(size_t entity_id)
{
    auto& entity = ecs_.get_component<ScriptingComponent>(entity_id);

    if (entity.has_value()) {
        loadScript(*entity);
    }
}

void ScriptingSystem::initAll() {
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

    LuaScript& lua_script = entity_scripts_[script_comp.entity_id];

    lua_State* L = lua_script.get_state();
    lua_pushinteger(L, script_comp.entity_id);
    lua_setglobal(L, "CURRENT_ENTITY");

    registerTransform(L, script_comp.entity_id);

    lua_script.run(loc_lua_file);
}

void ScriptingSystem::removeScript(size_t entity_id) {
    entity_scripts_.erase(entity_id);
}

void ScriptingSystem::update(float dt) {
    
    for (auto it = entity_scripts_.begin(); it != entity_scripts_.end(); ) {
        if (!ecs_.has_component<ScriptingComponent>(it->first)) {
            it = entity_scripts_.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto& [entity, lua_script] : entity_scripts_) {
        lua_script.call_function("update", dt);
    }
}


int ScriptingSystem::get_position_component(lua_State* lua_state, size_t entity) {
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

int ScriptingSystem::get_rotation_component(lua_State* lua_state, size_t entity) {
    if (ecs_.has_component<TransformComponent>(entity)) {
        auto& transf = ecs_.get_component<TransformComponent>(entity);
        
        glm::vec3 euler = glm::eulerAngles(transf->rotation);

        lua_pushnumber(lua_state, euler.y); 
        lua_pushnumber(lua_state, euler.x);  
        lua_pushnumber(lua_state, euler.z); 

        return 3;
    }
    luaL_error(lua_state, "Entity %d missing TransformComponent", entity);
    return 0;
}

int ScriptingSystem::get_scale_component(lua_State* lua_state, size_t entity) {
    if (ecs_.has_component<TransformComponent>(entity)) {
        auto& transf = ecs_.get_component<TransformComponent>(entity);
        lua_pushnumber(lua_state, transf->scale.x);
        lua_pushnumber(lua_state, transf->scale.y);
        lua_pushnumber(lua_state, transf->scale.z);
        return 3;
    }
    luaL_error(lua_state, "Entity %d missing TransformComponent", entity);
    return 0;
}

int ScriptingSystem::set_position_component(lua_State* lua_state, size_t entity) {
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

int ScriptingSystem::set_rotation_component(lua_State* lua_state, size_t entity) {
    
    float x = static_cast<float>(luaL_checknumber(lua_state, 1));
    float y = static_cast<float>(luaL_checknumber(lua_state, 2));
    float z = static_cast<float>(luaL_checknumber(lua_state, 3));

    glm::quat rotation = glm::quat(glm::vec3(x, y, z));

    if (ecs_.has_component<TransformComponent>(entity)) {
        auto& transf = ecs_.get_component<TransformComponent>(entity);
        transf->rotation = rotation;
        return 0;
    }

    luaL_error(lua_state, "Entity %d missing TransformComponent", entity);
    return 0;
}


int ScriptingSystem::set_scale_component(lua_State* lua_state, size_t entity) {
    glm::vec3 scale;

    scale.x = static_cast<float>(luaL_checknumber(lua_state, 1));
    scale.y = static_cast<float>(luaL_checknumber(lua_state, 2));
    scale.z = static_cast<float>(luaL_checknumber(lua_state, 3));

    if (ecs_.has_component<TransformComponent>(entity)) {
        auto& transf = ecs_.get_component<TransformComponent>(entity);
        transf->scale = scale;
        return 0;
    }

    luaL_error(lua_state, "Entity %d missing TransformComponent", entity);
    return 0;
}

void ScriptingSystem::registerTransform(lua_State* L, size_t entity_id) {

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->get_position_component(L, entity);
        }, 2);
    lua_setglobal(L, "getPosition");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity_id = static_cast<size_t>(luaL_checkinteger(L, 1));
        return self->get_position_component(L, entity_id);
        }, 1);
    lua_setglobal(L, "getPositionOf");

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->get_rotation_component(L, entity);
        }, 2);
    lua_setglobal(L, "getRotation");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity_id = static_cast<size_t>(luaL_checkinteger(L, 1));
        return self->get_rotation_component(L, entity_id);
        }, 1);
    lua_setglobal(L, "getRotationOf");

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->get_scale_component(L, entity);
        }, 2);
    lua_setglobal(L, "getScale");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity_id = static_cast<size_t>(luaL_checkinteger(L, 1));
        return self->get_scale_component(L, entity_id);
        }, 1);
    lua_setglobal(L, "getScaleOf");

    //------------------------------------

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->set_position_component(L, entity);
        }, 2);
    lua_setglobal(L, "setPosition");

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->set_rotation_component(L, entity);
        }, 2);
    lua_setglobal(L, "setRotation");

    lua_pushlightuserdata(L, this);
    lua_pushinteger(L, entity_id);
    lua_pushcclosure(L, [](lua_State* L) -> int {
        ScriptingSystem* self = static_cast<ScriptingSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
        size_t entity = static_cast<size_t>(lua_tointeger(L, lua_upvalueindex(2)));
        return self->set_scale_component(L, entity);
        }, 2);
    lua_setglobal(L, "setScale");
}

