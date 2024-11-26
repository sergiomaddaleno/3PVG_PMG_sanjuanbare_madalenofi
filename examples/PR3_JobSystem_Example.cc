
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

std::vector<std::string> meshFiles = {
        "../assets/obj/SM_Suzanne.obj",
        "../assets/obj/SM_Teapot.obj",
        "../assets/obj/Mario.obj",
};

std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

JobSystem jb;

Render render_;
ShaderBuffers shaders = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders);

int currentMeshIndex = 0;
bool loadingNewMesh = false;

int main(int, char**) {
    
    if (!mesh_->LoadOBj(meshFiles[currentMeshIndex],"l")) {
        return -1;
    }

    //---
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();

    auto render = render_.make();
    //---

    Input input{ window->GetWindow() };

    TransformComponent tr_;
    tr_.rotation = Vec3{ 0.0f, 1.0f, 0.0f };   

    //---


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
                if (newMesh->LoadOBj(nextMeshFile,"s")) {
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
    
        render_.SetupShader(shaders);
        render_.RenderMesh(mesh_, tr_);

        window->UpdateWindow();
       
        if (window->closePressed()) done = true;
    }

    return 0;
}
