
#include "imgui_manager.h"
#include "utils.h"
#include "global_variables.h"
#include "Render/post_processing.h"
#include "window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <../deps/glm/gtx/matrix_decompose.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

ImGuiManager::ImGuiManager() {}

ImGuiManager::~ImGuiManager() {
    Cleanup();
}

void ImGuiManager::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();
}

void ImGuiManager::Render(GLFWwindow* window, DataImgui& di) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImguiWindow(window);
    GameWindow(di);
    Hierarchy(di);
    Inspector(di);
    ContentBrowser(di);
    TopBar(di);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiManager::ImguiWindow(GLFWwindow* window) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    int framebuffer_width, framebuffer_height;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

    ImVec2 center_pos = ImVec2(framebuffer_width + framebuffer_width * 0.5f,
        framebuffer_height + framebuffer_height * 0.5f);

    ImGui::SetNextWindowPos(viewport->WorkPos); 
    ImGui::SetNextWindowSize(ImVec2((float)framebuffer_width, (float)framebuffer_height));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainDockspace_", nullptr, dockspace_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace_");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    static ImVec2 last_framebuffer_size = ImVec2(0, 0);
    ImVec2 current_framebuffer_size = ImVec2((float)framebuffer_width, (float)framebuffer_height);

    if (last_framebuffer_size.x != current_framebuffer_size.x || last_framebuffer_size.y != current_framebuffer_size.y) {
        last_framebuffer_size = current_framebuffer_size;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id);
        ImGui::DockBuilderSetNodeSize(dockspace_id, current_framebuffer_size);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_top = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up, 0.05f, nullptr, &dock_main);
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.1f, nullptr, &dock_main);
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
        ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.2f, nullptr, &dock_main);

        ImGui::DockBuilderDockWindow("##-TopBar", dock_top);
        ImGui::DockBuilderDockWindow("-Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("-Inspector", dock_right);
        ImGui::DockBuilderDockWindow("-ContentBrowser", dock_bottom);
        ImGui::DockBuilderDockWindow("-GameWindow", dock_main);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

void ImGuiManager::GameWindow(DataImgui& di) {
    ImGui::Begin("-GameWindow", nullptr);
    ImVec2 content_avail = ImGui::GetContentRegionAvail();

    GLuint captureTexture = di.game_texture;

    if (captureTexture) {
        ImVec2 uv0(0.0f, 1.0f);
        ImVec2 uv1(1.0f, 0.0f);

        ImGui::Image((ImTextureID)(intptr_t)captureTexture, content_avail, uv0, uv1);
    }

    ImGui::End();
}

void ImGuiManager::Hierarchy(DataImgui& di) {
    ImGui::Begin("-Hierarchy", nullptr);

    auto& loc_ecs = di.ecs;

    //--
    auto ri = loc_ecs->getComponentList<RenderComponent>().begin();
    auto ti = loc_ecs->getComponentList<TransformComponent>().begin();
    auto riend = loc_ecs->getComponentList<RenderComponent>().end();
    auto tiend = loc_ecs->getComponentList<TransformComponent>().end();

    int entity_id = 0;

    for (; ri != riend && ti != tiend; ++ri, ++ti, ++entity_id) {
        if (ti->has_value()) {

            if (ImGui::Selectable(("Entity " + std::to_string(entity_id)).c_str(), selected_entity == entity_id)) {
                selected_entity = entity_id;
            }
        }
    }
    //--

    ImGui::End();
}

void ImGuiManager::Inspector(DataImgui& di) {
    ImGui::Begin("-Inspector");

    Gizmo(di.cam.getViewMatrix(), di.cam.getProjectionMatrix(), selected_entity, di);

    ImGui::End();
}

void ImGuiManager::ContentBrowser(DataImgui& di) {
    ImGui::Begin("-ContentBrowser");

    ImGui::End();
}

void ImGuiManager::TopBar(DataImgui& di) {

    ImGui::Begin("##-TopBar", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_MenuBar);

    auto& loc_ecs = *(di.ecs);
    static int curr_type_draw = 0; 

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Settings")) {
            static float camSpeed = di.cam.getSpeed(); 
            static float camFov = di.cam.getFov();  
            static bool accept_button = false;

            if (ImGui::SliderFloat("Camera Speed", &camSpeed, 0.1f, 10.0f, "%.2f")) {
                accept_button = true;
            }

            if (ImGui::SliderFloat("Camera FOV", &camFov, 30.0f, 120.0f, "%.1f")) {
                accept_button = true;
            }

            if (ImGui::Button("Apply") && accept_button) {
                di.cam.setSpeed(camSpeed);
                di.cam.setFov(camFov);   
                accept_button = false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {

            if (ImGui::BeginMenu("Drawing Type")) {

                if (ImGui::MenuItem("Triangles", nullptr, curr_type_draw == 0)) {
                    curr_type_draw = 0;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
                if (ImGui::MenuItem("Lines", nullptr, curr_type_draw == 1)) {
                    curr_type_draw = 1;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                if (ImGui::MenuItem("Points", nullptr, curr_type_draw == 2)) {
                    curr_type_draw = 2;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Post Proccess")) {

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools")) {

            if (ImGui::MenuItem("Add Sphere")) {
                di.render_system.createSphere(loc_ecs, di.shader);

                di.render_system.initEntities(loc_ecs);
               
            }

            if (ImGui::MenuItem("Add Cube 8v")) {
                di.render_system.createCube8v(loc_ecs, di.shader);
                di.render_system.initEntities(loc_ecs);
            }

            if (ImGui::MenuItem("Add Cube 24v")) {
                di.render_system.createCube24v(loc_ecs, di.shader);
                di.render_system.initEntities(loc_ecs);
            }

            if (ImGui::MenuItem("Add Quad")) {
                di.render_system.createQuad(loc_ecs, di.shader);
                di.render_system.initEntities(loc_ecs);
            }

            if (ImGui::MenuItem("Add Terrain")) {
                di.render_system.createTerrain(loc_ecs, di.shader);
                di.render_system.initEntities(loc_ecs);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void ImGuiManager::Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity, DataImgui& di) {
    auto& loc_ecs = *(di.ecs);

    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    ImGuizmo::BeginFrame();
    if (ImGui::Begin("-GameWindow", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        auto ri = loc_ecs.getComponentList<RenderComponent>().begin();
        auto ti = loc_ecs.getComponentList<TransformComponent>().begin();

        int entity_id = 0;

        for (; ri != loc_ecs.getComponentList<RenderComponent>().end() &&
            ti != loc_ecs.getComponentList<TransformComponent>().end();
            ++ri, ++ti, ++entity_id) {
            if (entity_id == selected_entity && ri->has_value() && ti->has_value()) {
                auto& transform = loc_ecs.get_component<TransformComponent>(selected_entity);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform->position);
                model = model * glm::mat4_cast(transform->rotation);
                model = glm::scale(model, transform->scale);

                float objectMatrix[16];
                std::memcpy(objectMatrix, glm::value_ptr(model), sizeof(objectMatrix));

                ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                    mCurrentGizmoOperation, ImGuizmo::LOCAL,
                    objectMatrix, nullptr);

                if (ImGuizmo::IsUsing()) {
                    glm::mat4 newModelMatrix = glm::make_mat4(objectMatrix);
                    glm::vec3 scale, translation, skew;
                    glm::vec4 perspective;
                    glm::quat rotation;

                    glm::decompose(newModelMatrix, scale, rotation, translation, skew, perspective);

                    transform->position = translation;
                    transform->rotation = rotation;
                    transform->scale = scale;
                }
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("-Inspector", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        if (selected_entity >= 0) {
            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;
        }
    }
    ImGui::End();
}
