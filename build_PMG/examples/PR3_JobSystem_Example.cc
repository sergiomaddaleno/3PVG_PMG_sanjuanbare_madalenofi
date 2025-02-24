
#include "Core/window.h"
#include "Core/mesh.h"
#include "Systems/job_sistem.h"
#include "Core/input.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"

#include <../deps/glm/glm.hpp>
#include "utils.h"

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

auto mesh_ = std::make_unique<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

JobSystem jb;
RenderComponent rc;
TransformComponent tc;

char* vs_normals = (char*)ReadFile("../assets/shaders/normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/normals.fs");

int currentMeshIndex = 0;
bool loadingNewMesh = false;

int main(int, char**) {
    
    if (!mesh_->loadOBJ(meshFiles[currentMeshIndex],"l")) {
        return -1;
    }

    //---
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();
    //---

    Input input{ window->getWindow() };

    tc.rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };  
    tc.scale = glm::vec3{ 0.8f, 0.8f, 0.8f };
    tc.position = glm::vec3{ 0.0f, -2.0f, 0.0f };

    rc.shader.compile(vs_normals, fs_normals);
    //---

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();

        rc.mesh_ = std::move(mesh_);

        float time = (float)window->getTime() * 2;
        tc.rotation = glm::vec3{ 0.0f, time, 0.0f };
      
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
    
        rc.mesh_->render(rc, tc);

        if (window->closePressed()) done = true;
    }

    return 0;
}
