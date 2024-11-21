#include "window.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "ecs_manager.h"
#define M_PI 3.14159265358979323846

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"out vec3 normal;\n"
"out vec3 FragPos;\n"
"out vec3 pos;\n"
"out vec2 uv;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"    normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    pos = aPos;\n"
"    uv = aTexCoord;\n"
"    FragPos = aPos;\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 normal;\n"
"in vec3 FragPos;\n"
"in vec3 pos;\n"
"in vec2 uv;\n"
"void main() {\n"
"    vec3 norm = normalize(normal);\n"
"    FragColor = vec4(norm, 1.0);\n"
"}\n";

struct RenderComponent {
    std::shared_ptr<Mesh> render_mesh;
    

    RenderComponent() = default;

  
    RenderComponent(const RenderComponent& other)
        : render_mesh(other.render_mesh ? std::make_shared<Mesh>(*other.render_mesh) : nullptr) {}

   
    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            render_mesh = other.render_mesh ? std::make_shared<Mesh>(*other.render_mesh) : nullptr;
        }
        return *this;
    }
};

struct Vec3 {
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

   
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}
};

struct PositionComponent {
    
    Vec3 position;  

    
    PositionComponent() : position(0.0f, 0.0f, 0.0f) {}

    
    PositionComponent(const PositionComponent& other) : position(other.position) {}

    
    PositionComponent& operator=(const PositionComponent& other) {
        if (this != &other) {
            position = other.position;
        }
        return *this;
    }

   
    PositionComponent(const Vec3& newPosition) : position(newPosition) {}
};


void renderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<PositionComponent>>& pc) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_text, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_text, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    
    auto ri = rc.begin();
    auto pi = pc.begin();
    auto riend = rc.end();
    auto piend = pc.end();

    for (; ri != riend || pi != piend; ri++, pi++) {
        

        if (ri->has_value() && pi->has_value()) {


            auto& renderComponent = *ri;
            auto& positionComponent = *pi;


            auto& mesh = renderComponent->render_mesh;

            unsigned int VAO, VBO, EBO;
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

            // Configuración de atributos de vértices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);

            glUseProgram(shaderProgram);

            mat4x4 model, view, projection;
            mat4x4_identity(model);


            mat4x4_translate(model, positionComponent->position.x, positionComponent->position.y, positionComponent->position.z);


            float angle = (float)glfwGetTime();
            mat4x4_rotate(model, model, 0.0f, 1.0f, 0.0f, angle);

            mat4x4_identity(view);
            mat4x4_translate(view, 0.0f, -3.0f, -10.0f);

            mat4x4_perspective(projection, (float)(55.0f * (M_PI / 180.0f)), 800.0f / 600.0f, 0.1f, 100.0f);

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (const GLfloat*)model);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (const GLfloat*)view);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (const GLfloat*)projection);

            vec3 lightPos = { 1.0f, 1.0f, 1.0f };
            glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, (const GLfloat*)&lightPos);

            vec3 viewPos = { 1.0f, 1.0f, 3.0f };
            glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, (const GLfloat*)&viewPos);


            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glUseProgram(0);

            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    }
}

int main(int, char**) {

    std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();
    std::unique_ptr<Mesh> mesh_2 = std::make_unique<Mesh>();
    std::unique_ptr<Mesh> mesh_3 = std::make_unique<Mesh>();

    if (!mesh_->LoadOBj("../assets/SM_Teapot.obj")) {
        return -1;
    }

    if (!mesh_2->LoadOBj("../assets/SM_Suzanne.obj")) {
        return -1;
    }

    if (!mesh_3->LoadOBj("../assets/Mario.obj")) {
        return -1;
    }

    glfwInit();

    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    Render render_;
    auto render = render_.make();

    ECSManager ecs;

    auto entidad_0 = ecs.create_entity();
    auto entidad_1 = ecs.create_entity();
    auto entidad_2 = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<PositionComponent>();

    ecs.add_component<RenderComponent>(entidad_0);
    ecs.add_component<PositionComponent>(entidad_0);
    ecs.get_component<RenderComponent>(entidad_0)->render_mesh = std::move(mesh_);
    ecs.get_component<PositionComponent>(entidad_0)->position = Vec3{ 3.0f, -1.0f, -5.0f };

    ecs.add_component<RenderComponent>(entidad_1);
    ecs.add_component<PositionComponent>(entidad_1);
    ecs.get_component<RenderComponent>(entidad_1)->render_mesh = std::move(mesh_2);
    ecs.get_component<PositionComponent>(entidad_1)->position = Vec3{ -3.0f, 1.0f, -5.0f };

    ecs.add_component<RenderComponent>(entidad_2);
    ecs.add_component<PositionComponent>(entidad_2);
    ecs.get_component<RenderComponent>(entidad_2)->render_mesh = std::move(mesh_3);
    ecs.get_component<PositionComponent>(entidad_2)->position = Vec3{ 2.0f, 2.0f, -5.0f };

    bool done = false;
    while (!done) {
        window->UpdateWindow();
        renderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<PositionComponent>());
        if (window->closePressed()) done = true;
    }

    return 0;
}





