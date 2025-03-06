#ifndef __RENDER_COMPONENT__
#define __RENDER_COMPONENT__

#include "Geometry/triangle.h"
#include "Core/texture.h"
#include "Core/shader.h"
#include "Core/mesh.h"
#include "Render/light.h"
#include "Geometry/terrain.h"
#include "Geometry/sphere.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"

#include "../deps/nlohmann/json.hpp"

enum RenderType {
    RT_Mesh,
    RT_Terrain,
    RT_Sphere,
    RT_Cube8v,
    RT_Cube24v,
    RT_Quad,
    RT_None
};

struct RenderComponent {

    std::unique_ptr<Mesh> mesh_;
    Triangle triangle_;
    std::unique_ptr<Terrain>terrain_;
    std::unique_ptr<Sphere>sphere_;
    std::unique_ptr<Cube>cube_;
    std::unique_ptr<Quad>quad_;

    RenderType type_;

    // ---

    nlohmann::json serialize() const;

    RenderComponent();
    ~RenderComponent() {}
    
    RenderComponent& operator=(const RenderComponent& other);
    RenderComponent(const RenderComponent& other);
};

#endif // __RENDER_COMPONENT__
