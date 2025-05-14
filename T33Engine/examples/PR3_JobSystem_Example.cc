
#include "Core/window.h"
#include "Core/mesh.h"
#include "Systems/job_system.h"
#include "Core/input.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

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
        "../stages/assets/obj/SM_Teapot.obj",
        "../stages/assets/obj/Toad.obj",
        "../stages/assets/obj/SM_Suzanne.obj",
};

auto mesh_ = std::make_shared<Mesh>();
std::future<std::unique_ptr<Mesh>> monetemesh;

JobSystem jb;
RenderComponent rc;
TransformComponent tc;
Shader shader;
Camera cam;

int currentMeshIndex = 0;
bool loadingNewMesh = false;

void render(Shader shader, Mesh& mesh) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shader.useProgram();

    for (auto& submesh : mesh.meshes) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, tc.position);
        model = model * glm::mat4_cast(tc.rotation);
        model = glm::scale(model, tc.scale);

        glm::mat4 view = cam.getViewMatrix();
        glm::mat4 projection = cam.getPerspectiveMatrix();

        shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
        shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
        shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

        mesh.draw();
    }
}

int main(int, char**) {
    
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    if (!mesh_->loadOBJ(meshFiles[currentMeshIndex], "l")) {
        return -1;
    }

    initShader(shader, "../stages/shaders/basic/normals.vs", "../stages/shaders/basic/normals.fs");

    tc.position = glm::vec3{ 0.0f, 1.0f, -3.0f };
    tc.scale = glm::vec3{ 5.0f, 5.0f, 5.0f };

    mesh_->bind();

    cam.setViewDir(glm::vec3(0, 0, -1.0));
    cam.lockViewDirection(true);
    cam.lockMovement(true);

    //---

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();

        float time = (float)window->getTime() * 2;
        tc.rotation = glm::vec3{ 0.0f, time, 0.0f };
      
        mesh_ = mesh_;

        if (input.isKeyPressed(Input::RIGHT) && !loadingNewMesh) {

            mesh_ = mesh_;
            mesh_->bind();

            currentMeshIndex = (currentMeshIndex + 1) % meshFiles.size();
            std::string nextMeshFile = meshFiles[currentMeshIndex];

            loadingNewMesh = true;
            monetemesh = jb.Add_Task([nextMeshFile]() -> std::unique_ptr<Mesh> {

                std::unique_ptr<Mesh> newMesh = std::make_unique<Mesh>();
                if (newMesh->loadOBJ(nextMeshFile)) {
                    return newMesh;
                }
                return nullptr;
            });
        }

        if (loadingNewMesh && monetemesh.valid()) {
            std::unique_ptr<Mesh> loadedMesh = monetemesh.get();
            if (loadedMesh) {
                mesh_ = std::move(loadedMesh);
                mesh_->bind();
            }
            loadingNewMesh = false;
        }
    
        cam.update(input, dt);
        render(shader, *mesh_);

        if (window->closePressed(input)) done = true;
    }

    return 0;
}
