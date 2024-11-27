#ifndef RNC
#define RNC

#include <memory>

class Mesh;

struct RenderComponent {
    std::shared_ptr<Mesh> render_mesh;

    //GLuint textureID;
    //// Puntero a la malla que se va a renderizar
    //GLuint VAO = 0; // Inicializar VAO a 0
    //GLuint VBO = 0; // Inicializar VBO a 0
    //GLuint EBO = 0; // Inicializar EBO a 0

    RenderComponent() = default;

    RenderComponent(const RenderComponent& other)
        : render_mesh(other.render_mesh ? std::make_shared<Mesh>(*other.render_mesh) : nullptr)
        //VAO(0), VBO(0), EBO(0) 
        {} 

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            render_mesh = other.render_mesh ? std::make_shared<Mesh>(*other.render_mesh) : nullptr;

            /*VAO = 0;
            VBO = 0;
            EBO = 0;*/
        }
        return *this;
    }

    ~RenderComponent(){
        /*if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);*/
    }
        };

#endif
