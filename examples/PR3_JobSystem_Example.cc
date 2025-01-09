
#include "window.h"
#include "mesh.h"
#include "render.h"
#include "job_sistem.h"
#include "input.h"

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

std::shared_ptr<Mesh> mesh_ = std::make_shared<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

JobSystem jb;

Render render_;
char* vs_normals = (char*)ReadFile("../assets/shaders/vs_normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/fs_normals.fs");

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

    RenderData rd;
    rd.transf_comp.rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };  
    rd.transf_comp.scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
    rd.transf_comp.position = glm::vec3{ 0.0f, -3.0f, 0.0f };

    render_.setupShader(rd.render_comp.program_, vs_normals, fs_normals);

    //---

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();

        rd.render_comp.mesh_ = mesh_;

        float angle = (float)window->getTime() * 2;
        rd.transf_comp.angle = angle;
      
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
    
        render_.renderMesh(rd);

        if (window->closePressed()) done = true;
    }

    return 0;
}
