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
#include "Geometry/surface.h"

#include "../deps/nlohmann/json.hpp"

enum RenderType {
    RT_Mesh,
    RT_Terrain,
    RT_Sphere,
    RT_Cube8v,
    RT_Cube24v,
    RT_Quad,
    RT_Surface,
    RT_Water,
    RT_None
};

struct RenderComponent {

    RenderType type_;

    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Terrain>terrain_;
    std::shared_ptr<Terrain>water_;
    std::shared_ptr<Sphere>sphere_;
    std::shared_ptr<Cube>cube_;
    std::shared_ptr<Quad>quad_;
    std::shared_ptr<Surface>surface_;
    Triangle triangle_;

    //--

    nlohmann::json serialize() const;

    RenderComponent();
    ~RenderComponent() {}
    
    RenderComponent& operator=(const RenderComponent& other);
    RenderComponent(const RenderComponent& other);
};

#endif // __RENDER_COMPONENT__
