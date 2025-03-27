#include "Components/skybox_component.h"

nlohmann::json SkyboxComponent::serialize()
{
    nlohmann::json j;
    j["skybox"] = skybox.serialize();
    return j;
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
