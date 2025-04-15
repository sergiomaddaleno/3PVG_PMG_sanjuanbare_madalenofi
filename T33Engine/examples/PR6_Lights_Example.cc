
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "Core/window.h"
#include "Core/mesh.h"
#include "Render/render_system.h"
#include "Core/shader.h"
#include "Core/camera.h"
#include "Core/input.h"
#include "Systems/ecs_manager.h"
#include "utils.h"
#include "Components/light_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> light_bulb_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();


ECSManager ecs;
Camera cam;
Shader texture_shader, light_shader;

char* vs_texture = (char*)ReadFile("../stages/shaders/basic/texture.vs");
char* fs_texture = (char*)ReadFile("../stages/shaders/basic/texture.fs");

char* fs_light = (char*)ReadFile("../stages/shaders/forward_shading.fs");
char* vs_simple = (char*)ReadFile("../stages/shaders/forward_shading.vs");

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

            if (light_comp.light.type_ == Light::LightType::LT_Spot) {
                ImGui::Text(("Position" + id).c_str());
                ImGui::DragFloat(("Xp" + id).c_str(), &tr_comp.position.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Yp" + id).c_str(), &tr_comp.position.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Zp" + id).c_str(), &tr_comp.position.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text(("Angle" + id).c_str());
                ImGui::DragFloat(("angle" + id).c_str(), &light_comp.light.spot_angle_, 0.2f, -1000.0f, 100.0f, "%.3f");

                ImGui::Text(("Direction" + id).c_str());
                ImGui::DragFloat(("Xd" + id).c_str(), &light_comp.light.dir_.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Yd" + id).c_str(), &light_comp.light.dir_.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Zd" + id).c_str(), &light_comp.light.dir_.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text(("Light Color" + id).c_str());
                ImGui::DragFloat(("Xc" + id).c_str(), &light_comp.light.light_color_.x, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Yc" + id).c_str(), &light_comp.light.light_color_.y, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Zc" + id).c_str(), &light_comp.light.light_color_.z, 0.2f, 0.0f, 255.0f, "%.3f");
            }

            if (light_comp.light.type_ == Light::LightType::LT_Point) {
                ImGui::Text(("Position2" + id).c_str());
                ImGui::DragFloat(("Xp2" + id).c_str(), &tr_comp.position.x, 0.2f, -1000.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Yp2" + id).c_str(), &tr_comp.position.y, 0.2f, -100.0f, 100.0f, "%.3f");
                ImGui::DragFloat(("Zp2" + id).c_str(), &tr_comp.position.z, 0.2f, -100.0f, 100.0f, "%.3f");

                ImGui::Text(("Light Color2" + id).c_str());
                ImGui::DragFloat(("Xc2" + id).c_str(), &light_comp.light.light_color_.x, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Yc2" + id).c_str(), &light_comp.light.light_color_.y, 0.2f, 0.0f, 255.0f, "%.3f");
                ImGui::DragFloat(("Zc2" + id).c_str(), &light_comp.light.light_color_.z, 0.2f, 0.0f, 255.0f, "%.3f");
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

void renderScene(RenderComponent& rc, Shader& shader)
{
    switch (rc.type_) {
    case RenderType::RT_Mesh:
    {
        for (auto& mesh : rc.mesh_->meshes) {
            rc.mesh_->wave_.setUniforms(shader, (float)glfwGetTime());
            rc.mesh_->draw();
        }
    }
    break;
    case RenderType::RT_Terrain:
    {
        rc.terrain_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.terrain_->draw();
    }
    break;
    case RenderType::RT_Sphere:
    {
        rc.sphere_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.sphere_->draw();
    }
    break;
    case RenderType::RT_Surface:
    {
        rc.surface_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.surface_->draw();
    }
    break;
    case RenderType::RT_Cube8v:
    case RenderType::RT_Cube24v:
    {
        rc.cube_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.cube_->draw();
    }
    break;
    case RenderType::RT_Quad:
    {
        rc.quad_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.quad_->draw();

        int use = rc.quad_->use_normal_map_;
        shader.setUniformValue(shader.getUniformLocation("u_use_normal_map"),
            ValueType::T_INT_1, &use);

        use = rc.quad_->use_parallax_map_;
        shader.setUniformValue(shader.getUniformLocation("u_use_parallax_map"),
            ValueType::T_INT_1, &use);

        float scale = rc.quad_->parallax_scale_;
        shader.setUniformValue(shader.getUniformLocation("u_parallax_scale"),
            ValueType::T_FLOAT_1, &scale);

        int type = rc.quad_->parallax_type;
        shader.setUniformValue(shader.getUniformLocation("u_parallax_type"),
            ValueType::T_INT_1, &type);
    }
    break;

    }
}

void setUniforms(TransformComponent& tc, Shader& shader, Camera& cam)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = TextureID::TI_Diffuse;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), ValueType::T_INT_1, &tex_id);



}

void forwardRender(Shader shader, Camera cam) {

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    shader.useProgram();

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();

    for (; li != liend; ++li) {
        if (li->has_value()) {
            auto& light_comp = *li;


            shader.setUniformValue(shader.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));

            light_comp->light.setUniforms(shader);

            // ---------------------------------------------------------------

            auto ri = ecs.getComponentList<RenderComponent>().begin();
            auto ti = ecs.getComponentList<TransformComponent>().begin();
            auto riend = ecs.getComponentList<RenderComponent>().end();
            auto tiend = ecs.getComponentList<TransformComponent>().end();

            for (; ri != riend && ti != tiend; ++ri, ++ti) {
                if (ri->has_value() && ti->has_value()) {
                    auto& render_comp = *ri;
                    auto& transform_comp = *ti;

                    setUniforms(*transform_comp, shader, cam);
                    renderScene(*render_comp, shader);
                }
            }
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }
    glDisable(GL_BLEND);
    glUseProgram(0);
}

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();
    RenderSystem render_system{ecs};
    Input input{ window->getWindow() };

    // ---

    texture_shader.compile(vs_texture, fs_texture);
    light_shader.compile(vs_simple, fs_light);
    
    render_system.shader = light_shader;

    render_system.entity_ = ecs.create_entity()+1;

    ecs.add_component_type<IdentifierComponent>();
    ecs.add_component_type<HierarchyComponent>();
    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();
    ecs.add_component_type<LightComponent>();
    ecs.add_component_type<PhysicsComponent>();
    ecs.add_component_type<CameraComponent>();
    ecs.add_component_type<SkyboxComponent>();
    ecs.add_component_type<ScriptingComponent>();

    render_system.createMesh("../stages/assets/obj/Mario.obj", "../stages/assets/textures/mario");

    //render_system.createPointLight(ecs);
    render_system.createSpotLight();
    //render_system.createDirectionalLight(ecs, light_shader);

    //---
    
    render_system.initRenderTypes();
    render_system.initLights();

    InitImGui(window->getWindow());
    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->colorWindow(color);

        cam.update(input, dt, window->getWidth(), window->getHeight());

        forwardRender(light_shader,cam);

        MyImGui(ecs);

        if (window->closePressed(input)) done = true;
    }

    return 0;
}





