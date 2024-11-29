#ifndef RNC
#define RNC

#include <memory>
#include "..\include\render.h"
#include "triangle.h"

class Mesh;

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Triangle> triangle_;

    GLuint program_;
    GLuint VAO=0, VBO=0, EBO=0;
    
    GLuint textureID;

    RenderComponent() = default;

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr),
        VAO(0), VBO(0), EBO(0), triangle_(other.triangle_)
    {}

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;

            VAO = 0;
            VBO = 0;
            EBO = 0;

            triangle_ = other.triangle_;
        }
        return *this;
    }

    ~RenderComponent() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    }
};

#endif
