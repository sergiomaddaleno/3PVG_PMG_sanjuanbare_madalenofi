#include "Components/camera_component.h"
#include "utils.h"

nlohmann::json CameraComponent::serialize() {
    nlohmann::json j;
    j["camera"] = camera.serialize();
    return j;
}

CameraComponent::CameraComponent()
{
}

CameraComponent::~CameraComponent()
{
}
