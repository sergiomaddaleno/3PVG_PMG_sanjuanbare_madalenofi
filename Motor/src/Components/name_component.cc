#include "Components/name_component.h"

nlohmann::json NameComponent::serialize()
{
	nlohmann::json json_data;
	json_data["Entity_name"] = name;
	return json_data;
}
