#include "Components/identifier_component.h"

nlohmann::json IdentifierComponent::serialize()
{
	nlohmann::json json_data;
	json_data["Entity_name"] = name;
	json_data["Entity_id"] = id;

	return json_data;
}

IdentifierComponent::IdentifierComponent() :
	id(0),
	name()
{
}

IdentifierComponent::~IdentifierComponent()
{
}
