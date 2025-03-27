#include "Components/skybox_component.h"

nlohmann::json SkyboxComponent::serialize()
{
	return nlohmann::json();
}

void SkyboxComponent::deserialize(const nlohmann::json& cam)
{
}

SkyboxComponent::SkyboxComponent()
{
}

SkyboxComponent::~SkyboxComponent()
{
}
