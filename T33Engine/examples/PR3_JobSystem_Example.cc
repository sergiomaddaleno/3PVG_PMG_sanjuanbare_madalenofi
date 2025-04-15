
#include "Core/window.h"
#include "Core/mesh.h"
#include "Systems/job_system.h"
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
        "../stages/assets/obj/LMcQueen.obj",
        "../stages/assets/obj/Toad.obj",
        "../stages/assets/obj/Mario.obj",
};

auto mesh_ = std::make_shared<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

JobSystem jb;
RenderComponent rc;
TransformComponent tc;

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
    tc.position = glm::vec3{ 0.0f, 3.0f, 0.0f };

    Shader shader_tex;

    initShader(shader_tex, "../stages/shaders/basic/normals.vs", "../stages/shaders/basic/normals.fs");
    //---

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();

        rc.mesh_ = mesh_;

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
            std::shared_ptr<Mesh> loadedMesh = monetemesh.get();
            if (loadedMesh) {

                mesh_ = loadedMesh;
                printf("Malla asignada\n");
            }
            else {
                printf("Error en asignar malla\n");
            }
            loadingNewMesh = false;
        }
    
        rc.mesh_->basicRender(shader_tex, rc,tc);

        if (window->closePressed(input)) done = true;
    }

    return 0;
}
