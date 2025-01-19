
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "mesh.h"
#include "render_system.h"
#include "input.h"
#include "ecs_manager.h"
#include "utils.h"
#include "shader.h"
#include "Components/light_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> light_bulb_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

RenderSystem render_system;
RenderData rd;
ECSManager ecs;
Shader texture_shader, light_shader, mesh_shader;

char* vs_texture = (char*)ReadFile("../assets/shaders/vs_texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/fs_texture.fs");
char* vs = (char*)ReadFile("../assets/shaders/vertex.vs");
char* fs = (char*)ReadFile("../assets/shaders/fragment.fs");

char* fs_light = (char*)ReadFile("../assets/shaders/fs_light.fs");
char* vs_light = (char*)ReadFile("../assets/shaders/vertex.vs");

int num_entities = 5000;
glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

void ChangeLight(ECSManager& ecs) {

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    for (; li != liend && ti != tiend; ++li, ++ti) {

        if (li->has_value() && ti->has_value()) {

            auto& light_comp = li->value();
            auto& tr_comp = ti->value();


            if (light_comp.light_info.type_ == LightType::LT_Spot) {
                ImGui::Text("Position");
                ImGui::DragFloat("Xp", &tr_comp.position.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat("Yp", &tr_comp.position.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat("Zp", &tr_comp.position.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text("Angle");
                ImGui::DragFloat("angle", &light_comp.light_info.spot_angle_, 0.2f, -1000.0f, 100.0f, "%.3f");

                ImGui::Text("Direction");
                ImGui::DragFloat("Xd", &light_comp.light_info.dir_.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat("Yd", &light_comp.light_info.dir_.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat("Zd", &light_comp.light_info.dir_.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text("Light Color");
                ImGui::DragFloat("Xc", &light_comp.light_info.light_color_.x, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat("Yc", &light_comp.light_info.light_color_.y, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat("Zc", &light_comp.light_info.light_color_.z, 0.2f, 0.0f, 255.0f, "%.3f");
            }

            if (light_comp.light_info.type_ == LightType::LT_Point) {
                ImGui::Text("Position2");
                ImGui::DragFloat("Xp2", &tr_comp.position.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat("Yp2", &tr_comp.position.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat("Zp2", &tr_comp.position.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text("Light Color2");
                ImGui::DragFloat("Xc2", &light_comp.light_info.light_color_.x, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat("Yc2", &light_comp.light_info.light_color_.y, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat("Zc2", &light_comp.light_info.light_color_.z, 0.2f, 0.0f, 255.0f, "%.3f");
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
    
    if (!light_bulb_mesh->loadOBJ("../assets/obj/light_bulb.obj")) {
        return -1;
    }

    texture_shader.compile(vs_texture, fs_texture);
    light_shader.compile(vs_light, fs_light);
    mesh_shader.compile(vs, fs);
    
    render_system.setupEntities(ecs);
    render_system.createMesh(ecs, mario_mesh, light_shader);

    render_system.createPointLight(ecs, light_bulb_mesh, mesh_shader, light_shader);
    //render_system.createSpotLight(ecs, light_bulb_mesh, mesh_shader, light_shader);
    //render_system.createDirectionalLight(ecs, light_bulb_mesh, mesh_shader, light_shader);

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

        rd.cam.update(input, dt, rd.w_width, rd.w_height);

        render_system.renderEntities(ecs, rd);

        MyImGui(ecs);

        if (window->closePressed()) done = true;
    }

    return 0;
}





