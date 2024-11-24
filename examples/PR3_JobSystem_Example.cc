
#include "window.h"
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\job_sistem.h"
#include <iostream>
#include <memory>
#include <future>
#include <vector>
#include <string>
#include <thread>
#include <optional>
#include <string>
#include "..\include\input.h"

#include "..\include\Components\transform_component.h"

static const char* vertex_shader_text_normals =
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

static const char* fragment_shader_text_normals =
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


std::vector<std::string> meshFiles = {
        "../assets/SM_Suzanne.obj",
        "../assets/SM_Teapot.obj",
        "../assets/Mario.obj",
};

std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

int currentMeshIndex = 0;
bool loadingNewMesh = false;

int main(int, char**) {
    
    if (!mesh_->LoadOBj(meshFiles[currentMeshIndex])) {
        return -1;
    }

    //---
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();
    //---

    Input input{ window->GetWindow() };
    Render r;
    auto render = r.make();

    JobSystem jb;

    TransformComponent tr_;
    tr_.rotation = Vec3{ 0.0f, 1.0f, 0.0f };   

    bool done = false;
    while (!done) {

        float angle = (float)glfwGetTime() * 2;
        tr_.angle = angle;

        input.pollEvents();
      
        if (input.isKeyPressed(Input::RIGHT) && !loadingNewMesh) {

            currentMeshIndex = (currentMeshIndex + 1) % meshFiles.size();
            std::string nextMeshFile = meshFiles[currentMeshIndex];

            loadingNewMesh = true;
            monetemesh = jb.Add_Task([nextMeshFile]() {

                std::unique_ptr<Mesh> newMesh = std::make_unique<Mesh>();
                if (newMesh->LoadOBj(nextMeshFile)) {
                    printf("Malla cargada: %s\n", nextMeshFile.c_str());
                    return newMesh;
                }
                else {
                    printf("Error malla: %s\n", nextMeshFile.c_str());
                    return std::unique_ptr<Mesh>(nullptr);
                }
                });
        }

        if (loadingNewMesh) {
            std::unique_ptr<Mesh> loadedMesh = monetemesh.get();
            if (loadedMesh) {

                mesh_ = std::move(loadedMesh);
                printf("Malla asignada\n");
            }
            else {
                printf("Error en asignar malla\n");
            }
            loadingNewMesh = false;
        }
    
        r.SetupShader(vertex_shader_text_normals, fragment_shader_text_normals);
        r.RenderMesh(mesh_, tr_);

        window->UpdateWindow();
       
        if (window->closePressed()) done = true;
    }

    return 0;
}
