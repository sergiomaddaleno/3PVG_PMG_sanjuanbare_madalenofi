#ifndef LCP
#define LCP

#include "light.h"
#include "Components/transform_component.h" 

struct LightComponent {

	std::shared_ptr<Light> light_;

	Shader shader;
};

#endif //LCP