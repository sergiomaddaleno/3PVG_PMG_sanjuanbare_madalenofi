#include "Components/light_component.h"

nlohmann::json LightComponent::serialize()
{
	nlohmann::json json_data;
	json_data["light"] = light.serialize();
	return json_data;
}
