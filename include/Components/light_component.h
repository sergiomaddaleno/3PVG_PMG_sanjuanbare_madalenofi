#ifndef LCP
#define LCP

#include <memory>
#include "shader.h"

class Light;

struct LightComponent {

	std::shared_ptr<Light> light_;

	Shader shader;
};

#endif //LCP