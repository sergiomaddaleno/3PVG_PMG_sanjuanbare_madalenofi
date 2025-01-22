
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "Render/mesh.h"
#include "Render/render_system.h"
#include "Render/shader.h"
#include "Render/camera.h"
#include "input.h"
#include "Systems/ecs_manager.h"
#include "utils.h"
#include "Components/light_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> light_bulb_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

RenderSystem render_system;
ECSManager ecs;
Camera cam;
Shader texture_shader, light_shader;

char* vs_texture = (char*)ReadFile("../assets/shaders/texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/texture.fs");

char* fs_light = (char*)ReadFile("../assets/shaders/light.fs");
char* vs_simple = (char*)ReadFile("../assets/shaders/simple_vertex.vs");

glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

void ChangeLight(ECSManager& ecs) {

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    int index = 0; 

    for (; li != liend && ti != tiend; ++li, ++ti, ++index) {

        if (li->has_value() && ti->has_value()) {

            auto& light_comp = li->value();
            auto& tr_comp = ti->value();

            std::string id = "##" + std::to_string(index);

            if (light_comp.light_info.type_ == LightType::LT_Spot) {
                ImGui::Text(("Position" + id).c_str());
                ImGui::DragFloat(("Xp" + id).c_str(), &tr_comp.position.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Yp" + id).c_str(), &tr_comp.position.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Zp" + id).c_str(), &tr_comp.position.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text(("Angle" + id).c_str());
                ImGui::DragFloat(("angle" + id).c_str(), &light_comp.light_info.spot_angle_, 0.2f, -1000.0f, 100.0f, "%.3f");

                ImGui::Text(("Direction" + id).c_str());
                ImGui::DragFloat(("Xd" + id).c_str(), &light_comp.light_info.dir_.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Yd" + id).c_str(), &light_comp.light_info.dir_.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Zd" + id).c_str(), &light_comp.light_info.dir_.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text(("Light Color" + id).c_str());
                ImGui::DragFloat(("Xc" + id).c_str(), &light_comp.light_info.light_color_.x, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Yc" + id).c_str(), &light_comp.light_info.light_color_.y, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Zc" + id).c_str(), &light_comp.light_info.light_color_.z, 0.2f, 0.0f, 255.0f, "%.3f");
            }

            if (light_comp.light_info.type_ == LightType::LT_Point) {
                ImGui::Text(("Position2" + id).c_str());
                ImGui::DragFloat(("Xp2" + id).c_str(), &tr_comp.position.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Yp2" + id).c_str(), &tr_comp.position.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Zp2" + id).c_str(), &tr_comp.position.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text(("Light Color2" + id).c_str());
                ImGui::DragFloat(("Xc2" + id).c_str(), &light_comp.light_info.light_color_.x, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Yc2" + id).c_str(), &light_comp.light_info.light_color_.y, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Zc2" + id).c_str(), &light_comp.light_info.light_color_.z, 0.2f, 0.0f, 255.0f, "%.3f");
            }
        }
    }
}


void InitImGui(GLFWwindow* w) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    int width, height;
    glfwGetFramebufferSize(w, &width, &height);
    io.DisplaySize = ImVec2((float)width, (float)height);

    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void MyImGui(ECSManager& ecs) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Menu");

    ChangeLight(ecs);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    // ---

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    texture_shader.compile(vs_texture, fs_texture);
    light_shader.compile(vs_simple, fs_light);
    
    render_system.setupEntities(ecs);
    render_system.createMesh(ecs, mario_mesh, light_shader);

    render_system.createPointLight(ecs, light_shader);
    render_system.createSpotLight(ecs, light_shader);
    //render_system.createDirectionalLight(ecs, light_shader);

    //---
    
    render_system.initEntities(ecs);

    InitImGui(window->getWindow());
    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->colorWindow(color);

        cam.update(input, dt, window->getWidth(), window->getHeight());

        render_system.renderEntities(ecs, cam);

        MyImGui(ecs);

        if (window->closePressed()) done = true;
    }

    return 0;
}





