
#include "imgui_manager.h"
#include "utils.h"
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

    ImVec2 center_pos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
        viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

    // Set up the full-screen dockspace
    ImGui::SetNextWindowPos(center_pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainDockspace", nullptr, dockspace_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    static bool first_time = true;
    if (first_time) {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);

        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_top = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up, 0.07f, nullptr, &dock_main);
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, nullptr, &dock_main);
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.3f, nullptr, &dock_main);
        ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.2f, nullptr, &dock_main);


        ImGui::DockBuilderDockWindow("##TopBar", dock_top);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
        ImGui::DockBuilderDockWindow("ContentBrowser", dock_bottom);
        ImGui::DockBuilderDockWindow("GameWindow", dock_main);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

void ImGuiManager::GameWindow(DataImgui& di) {
    ImGui::Begin("GameWindow", nullptr);
    ImVec2 content_avail = ImGui::GetContentRegionAvail();

    GLuint gameTex = di.post_proccessing.getGamefBO();
    if (gameTex) {
        ImVec2 uv0(0.0f, 1.0f);
        ImVec2 uv1(1.0f, 0.0f);

        ImGui::Image((ImTextureID)(intptr_t)gameTex, content_avail, uv0, uv1);
    }

    ImGui::End();
}

void ImGuiManager::Hierarchy(DataImgui& di) {
    ImGui::Begin("Hierarchy");

    auto& loc_ecs = *(di.ecs);

    //--
    auto ri = loc_ecs.getComponentList<RenderComponent>().begin();
    auto ti = loc_ecs.getComponentList<TransformComponent>().begin();
    auto riend = loc_ecs.getComponentList<RenderComponent>().end();
    auto tiend = loc_ecs.getComponentList<TransformComponent>().end();

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
    ImGui::Begin("Inspector");

    Gizmo(di.cam.getViewMatrix(), di.cam.getProjectionMatrix(), di.rd.w_width, di.rd.w_height, selected_entity, di);

    ImGui::End();
}

void ImGuiManager::ContentBrowser(DataImgui& di) {
    ImGui::Begin("ContentBrowser");

    ImGui::End();
}

void ImGuiManager::TopBar(DataImgui& di) {
    ImGui::Begin("##TopBar", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static int curr_type_draw = 0;

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
            ImGui::EndMenu();
        }
    

    ImGui::End();
}


void ImGuiManager::Gizmo(const glm::mat4& view, const glm::mat4& projection, int display_w, int display_h, int selected_entity, DataImgui& di)
{

    auto& loc_ecs = *(di.ecs);

    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, (float)display_w, (float)display_h);

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

            ImGuizmo::SetRect(0, 0, (float)display_w, (float)display_h);

            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;

            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                mCurrentGizmoOperation, ImGuizmo::LOCAL,
                objectMatrix, nullptr);

            if (ImGuizmo::IsUsing()) {
                glm::mat4 newModelMatrix = glm::make_mat4(objectMatrix);
                glm::vec3 scale, translation, skew;
                glm::vec4 perspective;
                glm::quat rotation;

                // Decompose the matrix back into position, rotation, and scale
                glm::decompose(newModelMatrix, scale, rotation, translation, skew, perspective);

                transform->position = translation;
                transform->rotation = rotation;
                transform->scale = scale;
            }

            break;
        }
    }
}

