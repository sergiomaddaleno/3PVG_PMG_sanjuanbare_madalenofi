#include "Components/scripting_component.h"

nlohmann::json ScriptingComponent::serialize() const
{
    return {
        {"script_path", script_path},
        {"entity_id", entity_id}
    };
}

ScriptingComponent::ScriptingComponent()
	: entity_id(0)
{
}

ScriptingComponent::~ScriptingComponent()
{
}
