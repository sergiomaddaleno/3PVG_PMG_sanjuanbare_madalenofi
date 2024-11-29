
#include "window.h"
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\job_sistem.h"
#include "..\include\input.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"
#include <../deps/glm/glm.hpp>

#include <iostream>
#include <memory>
#include <future>
#include <vector>
#include <string>
#include <thread>
#include <optional>
#include <string>

std::vector<std::string> meshFiles = {
        "../assets/obj/SM_Suzanne.obj",
        "../assets/obj/SM_Teapot.obj",
        "../assets/obj/Mario.obj",
};

std::shared_ptr<Mesh> mesh_ = std::make_shared<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

JobSystem jb;

Render render_;
ShaderBuffers shaders = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders);

int currentMeshIndex = 0;
bool loadingNewMesh = false;

int main(int, char**) {
    
    if (!mesh_->loadOBJ(meshFiles[currentMeshIndex],"l")) {
        return -1;
    }

    //---
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();

    auto render = render_.make();
    //---

    Input input{ window->getWindow() };

    TransformComponent tr_;
    tr_.rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };  
    tr_.scale = glm::vec3{ 0.5f, 0.5f, 0.5f };
    tr_.position = glm::vec3{ 0.0f, -1.5f, 0.0f };

    RenderComponent rc_;
    //---

    bool done = false;
    while (!done) {

        rc_.mesh_ = mesh_;

        float angle = (float)glfwGetTime() * 2;
        tr_.angle = angle;

        input.pollEvents();
      
        if (input.isKeyPressed(Input::RIGHT) && !loadingNewMesh) {

            currentMeshIndex = (currentMeshIndex + 1) % meshFiles.size();
            std::string nextMeshFile = meshFiles[currentMeshIndex];

            loadingNewMesh = true;
            monetemesh = jb.Add_Task([nextMeshFile]() {

                std::unique_ptr<Mesh> newMesh = std::make_unique<Mesh>();
                if (newMesh->loadOBJ(nextMeshFile,"s")) {
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
            std::shared_ptr<Mesh> loadedMesh = monetemesh.get();
            if (loadedMesh) {

                mesh_ = std::move(loadedMesh);
                printf("Malla asignada\n");
            }
            else {
                printf("Error en asignar malla\n");
            }
            loadingNewMesh = false;
        }
    
        render_.setupShader(shaders);
        render_.renderMesh(rc_, tr_);

        window->updateWindow();
       
        if (window->closePressed()) done = true;
    }

    return 0;
}
