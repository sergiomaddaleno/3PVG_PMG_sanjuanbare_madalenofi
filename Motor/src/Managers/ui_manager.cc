
#include "Managers/ui_manager.h"
#include "utils.h"
#include "Render/post_processing.h"
#include "Components/render_component.h"
#include "Components/light_component.h"
#include "Components/transform_component.h"
#include "Components\name_component.h"

#include "Systems/ecs_manager.h"
#include "Render/render_system.h"
#include "Render/skybox.h"
#include "Algorithms/l_system.h"

#include "Managers/save_manager.h"
#include "Core/window.h"
#include "global_data.h"

#include <iostream>
#include <cmath>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>
#include <fstream>
#include <unordered_map>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <../deps/imfilebrowser.h>

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

ImGui::FileBrowser fileDialog;

UIManager::UIManager()
    : selected_entity(0),
    m_BaseDirectory("../"),
    m_CurrentDirectory(m_BaseDirectory),
    padding(16.0f),
    thumbnailSize(128.0f),
    post_process_window(false),
    skybox_window(false),
    console_window(false),
    fog_window(false),
    l_system_window(false),
    new_pp_shader(),
    m_PuzzleIcon(),
    m_DeleteIcon(),
    post_process_compiled(false),
    load_scene_window(false),
    add_mesh_window(false)
{}

UIManager::~UIManager() {
    Cleanup();
}
void UIManager::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::SetImguiTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Fondo y ventanas
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);

    // Texto
    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.45f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

    // Botones
    colors[ImGuiCol_Button] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Fondos de frames (para sliders, inputs, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.45f, 1.00f);

    // Barra de título
    colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // Sliders y checkmarks
    colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    style.FrameRounding = 0.1f;
    style.WindowRounding = 0.1f;
    style.ChildRounding = 0.1f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;

    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.ItemSpacing = ImVec2(7.0f, 5.0f);
    style.FramePadding = ImVec2(5.0f, 3.0f);
}

void UIManager::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    SetImguiTheme();

    io.Fonts->AddFontFromFileTTF("../stages/assets/fonts/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 15.0f);

    // Initialize content browser
    fileDialog.SetTitle("File Browser");
    fileDialog.SetTypeFilters({
        ".h", ".cc", ".hpp", ".cpp",
        ".png", ".jpg", ".mtl",
        ".lua", ".json", ".obj",
        ".vs", ".fs",
        });

    m_DirectoryIcon.loadTexture("../stages/assets/textures/imgui/DirectoryIcon.png");
    m_FileIcon.loadTexture("../stages/assets/textures/imgui/FileIcon.png");
    m_EntityIcon.loadTexture("../stages/assets/textures/imgui/cube.png");
    m_BackIcon.loadTexture("../stages/assets/textures/imgui/back.png");
    m_ToolsIcon.loadTexture("../stages/assets/textures/imgui/tool.png");
    m_CameraIcon.loadTexture("../stages/assets/textures/imgui/camera.png");
    m_TransformIcon.loadTexture("../stages/assets/textures/imgui/transform.png");
    m_SettingsIcon.loadTexture("../stages/assets/textures/imgui/setting.png");
    m_TagIcon.loadTexture("../stages/assets/textures/imgui/tag.png");
    m_LightIcon.loadTexture("../stages/assets/textures/imgui/light.png");
    m_RenderIcon.loadTexture("../stages/assets/textures/imgui/render.png");
    m_EditIcon.loadTexture("../stages/assets/textures/imgui/edit.png");
    m_PuzzleIcon.loadTexture("../stages/assets/textures/imgui/puzzle.png");
    m_DeleteIcon.loadTexture("../stages/assets/textures/imgui/delete.png");
    m_PlayIcon.loadTexture("../stages/assets/textures/imgui/play.png");
    m_StopIcon.loadTexture("../stages/assets/textures/imgui/stop.png");
    m_AlgorithmIcon.loadTexture("../stages/assets/textures/imgui/algorithm.png");

    //---

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

void UIManager::Render(GLFWwindow* window) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImguiWindow(window);

    if (!global.is_playing) {
        HierarchyWindow();
        InspectorWindow();
        ContentBrowserWindow();
        TopBarWindow();

        if (console_window) ConsoleWindow();
        if (post_process_window) PostProcessWindow();
        if (skybox_window) SkyboxWindow();
        if (fog_window) FogWindow();
        if (load_scene_window) LoadSceneWindow();
        if (add_mesh_window) AddMeshWindow();
        if (l_system_window) LSystemWindow();

        global.grid.drawGrid(global.cam);
    }

    global.post_process.endRender();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameWindow();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

//--
void UIManager::ImguiWindow(GLFWwindow* window) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    if (global.is_playing) {
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("Main", nullptr, window_flags);
        ImGui::End();
        return;
    }

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("Main", nullptr, dockspace_flags);

    ImGuiID dockspace_id = ImGui::GetID("Main");

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    static bool initialized = false;
    if (!initialized) {
        initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_top = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up, 0.05f, nullptr, &dock_main);
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.1f, nullptr, &dock_main);
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);
        ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.2f, nullptr, &dock_main);
        ImGuiID dock_bottom_left;
        ImGuiID dock_bottom_right = ImGui::DockBuilderSplitNode(dock_bottom, ImGuiDir_Right, 0.5f, &dock_bottom_left, &dock_bottom);

        ImGui::DockBuilderDockWindow("Menu", dock_top);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
        ImGui::DockBuilderDockWindow("ContentBrowser", dock_bottom);
        ImGui::DockBuilderDockWindow("GameWindow", dock_main);
        ImGui::DockBuilderDockWindow("PostProcess", dock_right);
        ImGui::DockBuilderDockWindow("Fog", dock_right);
        ImGui::DockBuilderDockWindow("SkyBox", dock_right);
        ImGui::DockBuilderDockWindow("Console", dock_bottom_right);
        ImGui::DockBuilderDockWindow("Load Scene", dock_right);
        ImGui::DockBuilderDockWindow("Add Mesh", dock_right); 
        ImGui::DockBuilderDockWindow("L-System", dock_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

void UIManager::GameWindow() {
    ImVec2 button_size = { 20.0f, 20.0f };
    ImVec2 button_pos;

    if (global.is_playing) {
        ImGui::Begin("Main", nullptr);
    }
    else {
        ImGui::Begin("GameWindow", nullptr);
    }

    ImVec2 window_size = ImGui::GetContentRegionAvail();

    if (global.is_playing) {
        button_pos = { (window_size.x - button_size.x) * 0.5f, 0.0f };
    }
    else {
        button_pos = { (window_size.x - button_size.x) * 0.5f, 25.0f };
    }

    ImGui::SetCursorPos(button_pos);

    ImTextureID icon = global.is_playing ? m_StopIcon.getID() : m_PlayIcon.getID();
    if (ImGui::ImageButton("Play", icon, button_size, { 0, 1 }, { 1, 0 })) {
        global.is_playing = !global.is_playing;
    }

    GLuint captureTexture = global.post_process.frame_buffer_.GetColorTexture();
    if (captureTexture) ImGui::Image((ImTextureID)(intptr_t)captureTexture, { window_size.x, window_size.y - (button_size.y + 7.0f)}, { 0.0f, 1.0f }, { 1.0f, 0.0f });

    ImGui::End();
}

void UIManager::HierarchyWindow() {
    ImGui::Begin("Hierarchy", nullptr);

    auto& loc_ecs = global.ecs;

    //--
    auto ri = loc_ecs.getComponentList<RenderComponent>().begin();
    auto ti = loc_ecs.getComponentList<TransformComponent>().begin();

    auto riend = loc_ecs.getComponentList<RenderComponent>().end();
    auto tiend = loc_ecs.getComponentList<TransformComponent>().end();

    auto li = loc_ecs.getComponentList<LightComponent>().begin();
    auto liend = loc_ecs.getComponentList<LightComponent>().end();

    auto ni = loc_ecs.getComponentList<NameComponent>().begin();
    auto niend = loc_ecs.getComponentList<NameComponent>().end();

    int entity_id = 0;
    

    for (; ni != niend; ++ni, entity_id++) {

        if (ni->has_value() && ni != niend) {
            auto& name_comp = *ni;

            ImGui::Image(m_EntityIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            ImGui::SameLine();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);

            if (ImGui::Selectable(name_comp->name.c_str(), selected_entity == entity_id))
                selected_entity = entity_id;
        }
    }
    

    ImGui::End();
}

void UIManager::InspectorWindow() {
    ImGui::Begin("Inspector");

    if (global.ecs.has_component<NameComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##NameComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_TagIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Name Component");

        if (open) {
            auto& name_comp = global.ecs.get_component<NameComponent>(selected_entity);
            ImGui::Dummy({ 0.0f, 3.0f });

            if (name_comp->name.empty()) name_comp->name = "Entity"; 

            if (ImGui::InputText("##NameInput", &name_comp->name)) {
                if (name_comp->name.empty()) name_comp->name = "Entity";
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

    }

    if (global.ecs.has_component<TransformComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##TransformComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_TransformIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Transform Component");

        if (open) {
            auto& transf_comp = global.ecs.get_component<TransformComponent>(selected_entity);

            if (global.cam.getUsePerspectiveMatrix()) {
                ImGuizmo::SetOrthographic(false);
                Gizmo(global.cam.getViewMatrix(), global.cam.getPerspectiveMatrix(), selected_entity);
            }
            else {
                ImGuizmo::SetOrthographic(true);
                Gizmo(global.cam.getViewMatrix(), global.cam.getOrthograficMatrix(), selected_entity);
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            ImGui::Columns(4, "TransformColumns", true);

            ImGui::TextUnformatted(" ");            
            ImGui::NextColumn();

            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "X"); 
            ImGui::NextColumn();

            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Y"); 
            ImGui::NextColumn();

            ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Z");  
            ImGui::NextColumn();

            auto DrawRow = [&](const char* label, float& x, float& y, float& z, const char* id)
                {
                    ImGui::TextUnformatted(label);
                    ImGui::NextColumn();

                    ImGui::DragFloat((std::string("##X") + id).c_str(), &x, 0.1f, -1000.0f, 1000.0f);
                    ImGui::NextColumn();

                    ImGui::DragFloat((std::string("##Y") + id).c_str(), &y, 0.1f, -1000.0f, 1000.0f);
                    ImGui::NextColumn();

                    ImGui::DragFloat((std::string("##Z") + id).c_str(), &z, 0.1f, -1000.0f, 1000.0f);
                    ImGui::NextColumn();
                };

            DrawRow("Translation", transf_comp->position.x, transf_comp->position.y, transf_comp->position.z, "Translation");

            glm::vec3 euler_angles = glm::degrees(glm::eulerAngles(transf_comp->rotation));
            DrawRow("Rotation   ", euler_angles.x, euler_angles.y, euler_angles.z, "Rotation");
            transf_comp->rotation = glm::quat(glm::radians(euler_angles)); 

            DrawRow("Scale      ", transf_comp->scale.x, transf_comp->scale.y, transf_comp->scale.z, "Scale");

            ImGui::Columns(1);
            ImGui::Spacing();
            

        }
    }

    if (global.ecs.has_component<RenderComponent>(selected_entity)) {

        auto& render_comp = global.ecs.get_component<RenderComponent>(selected_entity);

        bool open = ImGui::CollapsingHeader("##RenderComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_RenderIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Render Component");

        if (open) {

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
            ImGui::Text("Change Shader");
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::PopStyleColor();

            static std::string vertex_file_path;
            static std::string vertex_file_name;
            if (ImGui::Button(!vertex_file_path.empty() ? vertex_file_name.c_str() : ".vs Drop here", { 120.0f, 40.0f })) {}

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    std::filesystem::path filePath = (const wchar_t*)payload->Data;
                    if (filePath.extension() == ".vs") {
                        vertex_file_path = filePath.string();
                        vertex_file_name = filePath.filename().string();
                    }
                }
                ImGui::EndDragDropTarget();
            }

            static std::string fragment_file_path;
            static std::string fragment_file_name;
            ImGui::SameLine();
            if (ImGui::Button(!fragment_file_path.empty() ? fragment_file_name.c_str() : ".fs Drop here", { 120.0f, 40.0f })) {}

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    std::filesystem::path filePath = (const wchar_t*)payload->Data;
                    if (filePath.extension() == ".fs") {
                        fragment_file_path = filePath.string();
                        fragment_file_name = filePath.filename().string();
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::Button("Compile Shader")) {
                
                initShader(global.render_system.shader, vertex_file_path, fragment_file_path);

                vertex_file_path.clear();
                vertex_file_name.clear();
                fragment_file_path.clear();
                fragment_file_name.clear();
            }
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // -----------------------------------------------------------------

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.3f, 0.2f, 1.0f));
            ImGui::Text("Texture settings");
            ImGui::PopStyleColor();
            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            switch (render_comp->type_) {
                case RenderType::RT_Mesh:
                    ImGui::Checkbox("Use Texture", &render_comp->mesh_->use_texture_);
                    ImGui::ColorEdit3("Color", glm::value_ptr(render_comp->mesh_->color_));
                    break;
                case RenderType::RT_Terrain:
                    ImGui::Checkbox("Use Texture", &render_comp->terrain_->use_texture_);
                    ImGui::ColorEdit3("Color", glm::value_ptr(render_comp->terrain_->color_));
                    break;
                case RenderType::RT_Sphere:
                    ImGui::Checkbox("Use Texture", &render_comp->sphere_->use_texture_);
                    ImGui::ColorEdit3("Color", glm::value_ptr(render_comp->sphere_->color_));
                    break;
                case RenderType::RT_Quad:
                    ImGui::Checkbox("Use Texture", &render_comp->quad_->use_texture_);
                    ImGui::ColorEdit3("Color", glm::value_ptr(render_comp->quad_->color_));
                    break;
                case RenderType::RT_Cube8v:
                case RenderType::RT_Cube24v:
                    ImGui::Checkbox("Use Texture", &render_comp->cube_->use_texture_);
                    ImGui::ColorEdit3("Color", glm::value_ptr(render_comp->cube_->color_));
                    break;
            }
            
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.3f, 0.0f, 1.0f));
            ImGui::Text("Change Texture");
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::PopStyleColor();

            static std::string texture_file_path;
            static std::string texture_file_name;
            if (ImGui::Button(!texture_file_path.empty() ? texture_file_name.c_str() : ".png / .jpg drop here", { 150.0f, 40.0f })) {}

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    std::filesystem::path filePath = (const wchar_t*)payload->Data;
                    if ((filePath.extension() == ".png") || (filePath.extension() == ".jpg")) {
                        texture_file_path = filePath.string();
                        texture_file_name = filePath.filename().string();
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::Button("Compile Texture")) {
                
                switch (render_comp->type_) {
                case RenderType::RT_Mesh:
                    break;

                case RenderType::RT_Terrain:
                    render_comp->terrain_->setTexture(texture_file_path);
                    break;

                case RenderType::RT_Sphere:
                    render_comp->sphere_->setTexture(texture_file_path);
                    break;

                case RenderType::RT_Quad:
                    render_comp->quad_->setTexture(texture_file_path);
                    break;

                case RenderType::RT_Cube8v:
                case RenderType::RT_Cube24v:
                    render_comp->cube_->setTexture(texture_file_path);
                    break;
                }

                texture_file_path.clear();
                texture_file_name.clear();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            // -----------------------------------------------------------------

            switch (render_comp->type_) {
            case ::RT_Terrain:
            {
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Text("Terrain Dimensions");
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                auto& terrain = *global.ecs.get_component<RenderComponent>(selected_entity)->terrain_;

                static int temp_num_cols = terrain.num_cols_;
                static int temp_num_rows = terrain.num_rows_;
                static float temp_hm = terrain.height_mult_;
                static float temp_size = terrain.size_;
                static bool temp_is_centered = terrain.is_centered_;
                static bool temp_use_pn = terrain.use_perlin_noise_;

                ImGui::InputInt("Number of Columns", &temp_num_cols);
                ImGui::InputInt("Number of Rows", &temp_num_rows);
                ImGui::SliderFloat("Height Multiplier", &temp_hm, 0.0f, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderFloat("Size", &temp_size, 0.0f, 10.0f, "%.3f");
                ImGui::Checkbox("Is Centered", &temp_is_centered);
                ImGui::Checkbox("Perlin Noise / Heightmap", &temp_use_pn);

                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                static std::string hm_file_path;
                static std::string hm_file_name;
                if (ImGui::Button(!hm_file_path.empty() ? hm_file_name.c_str() : "Heightmap drop here", { 200.0f, 40.0f })) {}

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                        std::filesystem::path filePath = (const wchar_t*)payload->Data;
                        if ((filePath.extension() == ".png") || (filePath.extension() == ".jpg")) {
                            hm_file_path = filePath.string();
                            hm_file_name = filePath.filename().string();
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                if (ImGui::Button("Apply Changes")) {

                    if (!hm_file_path.empty()) {
                        render_comp->terrain_->setHeightMap(hm_file_path);
                    }

                    terrain.init(temp_num_cols, temp_num_rows, temp_hm,
                        temp_size, temp_is_centered, temp_use_pn);

                    terrain.bind();
                }
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                break;
            }
            case ::RT_Sphere:
            {
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Text("Sphere Dimensions");
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                auto& sphere = *global.ecs.get_component<RenderComponent>(selected_entity)->sphere_;

                static int temp_num_heights = sphere.num_heights_;
                static int temp_num_revs = sphere.num_revs_;
                static float temp_size = sphere.size_;

                ImGui::InputInt("Number of Heights", &temp_num_heights);
                ImGui::InputInt("Number of Revolutions", &temp_num_revs);
                ImGui::SliderFloat("Size", &temp_size, 0.0f, 10.0f, "%.3f");

                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                if (ImGui::Button("Apply Changes")) {
                    sphere.init(temp_size, temp_num_heights, temp_num_revs);
                    sphere.bind();
                }

                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                break;
            }
            case ::RT_Cube8v:
            {
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Text("Cube8v Dimensions");
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                auto& cube = *global.ecs.get_component<RenderComponent>(selected_entity)->cube_;
                static float temp_size = cube.size_;
                static bool temp_is_centered = cube.is_centered_;

                ImGui::SliderFloat("Size", &temp_size, 0.0f, 10.0f, "%.3f");
                ImGui::Checkbox("Is Centered", &temp_is_centered);

                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                if (ImGui::Button("Apply Changes")) {
                    cube.init8v(temp_size, temp_is_centered);
                    cube.bind();
                }

                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                break;
            }
            case ::RT_Cube24v:
            {
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Text("Cube24v Dimensions");
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                auto& cube = *global.ecs.get_component<RenderComponent>(selected_entity)->cube_;
                static float temp_size = cube.size_;
                ImGui::SliderFloat("Size", &temp_size, 0.0f, 10.0f, "%.3f");

                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                if (ImGui::Button("Apply Changes")) {
                    cube.init24v(temp_size);
                    cube.bind();
                }

                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                break;
            }
            case ::RT_Quad:
            {
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Text("Quad Dimensions");
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                auto& quad = *global.ecs.get_component<RenderComponent>(selected_entity)->quad_;

                static float temp_size = quad.size_;
                ImGui::SliderFloat("Size", &temp_size, 0.0f, 10.0f, "%.3f");

                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                if (ImGui::Button("Apply Changes")) {
                    quad.init(temp_size);
                    quad.bind();
                }

                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                break;
            }
            }
        
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
           
            // -----------------------------------------------------------------   

            switch (render_comp->type_) {
            case ::RT_Terrain:
                {
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                ImGui::Text("Wave Options");
                ImGui::PopStyleColor();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                auto& terrain = *global.ecs.get_component<RenderComponent>(selected_entity)->terrain_;
                ImGui::Checkbox("Use Wave Effect", &terrain.wave_.use_wave_);

                ImGui::SliderFloat("Amplitude", &terrain.wave_.amplitude_, 0.0f, 1.0f);
                ImGui::SliderFloat("Length", &terrain.wave_.wave_length_, 0.1f, 10.0f);
                ImGui::SliderFloat("Speed", &terrain.wave_.speed_, 0.1f, 5.0f);

                ImGui::SliderFloat("Direction X", &terrain.wave_.direction_.x, -1.0f, 1.0f);
                ImGui::SliderFloat("Direction Y", &terrain.wave_.direction_.y, -1.0f, 1.0f);
                }
                break;
            }
        }
    }

    if (global.ecs.has_component<LightComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##LightComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_LightIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Light Component");

        if (open) {
            auto& light_comp = global.ecs.get_component<LightComponent>(selected_entity);

            if (light_comp->light.type_ == Light::LightType::LT_Directional) {
                ImGui::Dummy(ImVec2(0.0f, 5.0f));

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
                ImGui::Text("Directional direction");
                ImGui::PopStyleColor();
                ImGui::SliderAngle("X", &light_comp->light.dir_[0], -57.0f, 57.0f);
                ImGui::SliderAngle("Y", &light_comp->light.dir_[1], -57.0f, 57.0f);
                ImGui::SliderAngle("Z", &light_comp->light.dir_[2], -57.0f, 57.0f);

            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f)); 
            ImGui::Text("Light Colors");
            ImGui::PopStyleColor();

            glm::vec3& diff_color_ = light_comp->light.diff_color_;

            float diff[3] = {
                diff_color_.r / 255.0f,
                diff_color_.g / 255.0f,
                diff_color_.b / 255.0f
            };

            if (ImGui::ColorEdit3("Diffuse Color", diff, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel)) {
                diff_color_.r = diff[0] * 255.0f;
                diff_color_.g = diff[1] * 255.0f;
                diff_color_.b = diff[2] * 255.0f;
            }

            glm::vec3& spec_color_ = light_comp->light.spec_color_;

            float spec[3] = {
                spec_color_.r / 255.0f,
                spec_color_.g / 255.0f,
                spec_color_.b / 255.0f
            };
            
            if (ImGui::ColorEdit3("Specular Color", spec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel)) {
                spec_color_.r = spec[0] * 255.0f;
                spec_color_.g = spec[1] * 255.0f;
                spec_color_.b = spec[2] * 255.0f;
            }
           
            glm::vec3& light_color = light_comp->light.light_color_;

            float color[3] = {
                light_color.r / 255.0f,
                light_color.g / 255.0f,
                light_color.b / 255.0f
            };

            if (ImGui::ColorEdit3("Light Color", color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel)) {
                light_color.r = color[0] * 255.0f;
                light_color.g = color[1] * 255.0f;
                light_color.b = color[2] * 255.0f;
            }

            glm::vec3& ambient_color_ = light_comp->light.ambient_color_;

            float ambient[3] = {
                ambient_color_.r / 255.0f,
                ambient_color_.g / 255.0f,
                ambient_color_.b / 255.0f
            };
           
            if (ImGui::ColorEdit3("Ambient Color", ambient, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel)) {
                ambient_color_.r = ambient[0] * 255.0f;
                ambient_color_.g = ambient[1] * 255.0f;
                ambient_color_.b = ambient[2] * 255.0f;
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.9f, 0.5f, 1.0f)); 
            ImGui::Text("Attenuation");
            ImGui::PopStyleColor();

            ImGui::SliderFloat("Linear Attenuation", &light_comp->light.linear_att, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic Attenuation", &light_comp->light.quad_att_, 0.0f, 1.0f);
            ImGui::SliderFloat("Constant Attenuation", &light_comp->light.constant_att_, 0.0f, 1.0f);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 1.0f, 1.0f)); 
            ImGui::Text("Spotlight Settings");
            ImGui::PopStyleColor();

            ImGui::SliderFloat("Spot Angle", &light_comp->light.spot_angle_, 0.0f, 90.0f);
            ImGui::SliderFloat("Spot Softness", &light_comp->light.spot_softness_, 0.0f, 90.0f);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
            ImGui::Text("Shininess & Strength");
            ImGui::PopStyleColor();

            ImGui::SliderFloat("Shininess", &light_comp->light.shininess_, 1.0f, 128.0f);
            ImGui::SliderFloat("Strength", &light_comp->light.strength_, 0.0f, 10.0f);


            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::Text("Shadow Map");
            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            static float near_plane = light_comp->shadow_map_.near_plane;
            ImGui::SliderFloat("Near Plane", &near_plane, 0.1f, 100.0f);

            static float far_plane = light_comp->shadow_map_.far_plane;
            ImGui::SliderFloat("Far Plane", &far_plane, 10.0f, 1000.0f);

            static int shadow_width = light_comp->shadow_map_.shadow_width_;
            static int shadow_height = light_comp->shadow_map_.shadow_height_;
            ImGui::SliderInt("Shadow Width", &shadow_width, 512, 8192);
            ImGui::SliderInt("Shadow Height", &shadow_height, 512, 8192);

            static float directional_size = light_comp->shadow_map_.directional_size_;
            if (light_comp->light.type_ == Light::LightType::LT_Directional) {
                ImGui::SliderFloat("Directional Size", &directional_size, 10.0f, 1000.0f);
            }

            ImGui::Dummy({ 0.0f, 5.0f });

            if (ImGui::Button("Apply Shadow Map")) {
                light_comp->shadow_map_.near_plane = near_plane;
                light_comp->shadow_map_.far_plane = far_plane;
                light_comp->shadow_map_.shadow_width_ = shadow_width;
                light_comp->shadow_map_.shadow_height_ = shadow_height;
                light_comp->shadow_map_.directional_size_ = directional_size;

                light_comp->shadow_map_.init();
            }
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
        }
    }
    
    // ---

    bool open = ImGui::CollapsingHeader("##Components", ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::SameLine();
    ImGui::Image(m_PuzzleIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
    ImGui::SameLine();
    ImGui::Text("Components");

    if (open) {
        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        ImGui::Text("Entity has:");
        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        if (global.ecs.has_component<TransformComponent>(selected_entity)) {
            ImGui::Text("- Transform component");
        }
        if (global.ecs.has_component<LightComponent>(selected_entity)) {
            ImGui::Text("- Light component");
        }
        if (global.ecs.has_component<RenderComponent>(selected_entity)) {
            ImGui::Text("- Render component");
        }
        if (global.ecs.has_component<NameComponent>(selected_entity)) {
            ImGui::Text("- Name component");
        }
        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        const char* addComponents[] = { "Transform Component", "Light Component", "Render Component" };
        static int addCurrentItem = -1;

        if (ImGui::BeginCombo("Add Component", addCurrentItem >= 0 ? addComponents[addCurrentItem] : "Select Component to Add")) {
            for (int i = 0; i < IM_ARRAYSIZE(addComponents); i++) {
                bool hasComponent = false;
                switch (i) {
                case 0: hasComponent = global.ecs.has_component<TransformComponent>(selected_entity); break;
                case 1: hasComponent = global.ecs.has_component<LightComponent>(selected_entity); break;
                case 2: hasComponent = global.ecs.has_component<RenderComponent>(selected_entity); break;
                }

                if (!hasComponent) {
                    if (ImGui::Selectable(addComponents[i], addCurrentItem == i)) {
                        switch (i) {
                        case 0: global.ecs.add_component<TransformComponent>(selected_entity); break;
                        case 1: global.ecs.add_component<LightComponent>(selected_entity); 
                        {
                            auto& light_comp = global.ecs.get_component<LightComponent>(selected_entity);
                            light_comp->light.type_ = Light::LightType::LT_Point;
                            light_comp->light.pos_ = global.ecs.get_component<TransformComponent>(selected_entity)->position;
                            light_comp->light.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
                            light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
                            light_comp->light.linear_att = 0.7f;
                            light_comp->light.quad_att_ = 1.8f;
                            light_comp->light.constant_att_ = 1.0f;
                            light_comp->light.shininess_ = 32.0f;
                            light_comp->light.strength_ = 1.0f;
                        }
                        break;
                        case 2: global.ecs.add_component<RenderComponent>(selected_entity); break;
                        }
                        addCurrentItem = -1; 
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        const char* deleteComponents[] = { "Transform Component", "Light Component", "Render Component" };
        static int deleteCurrentItem = -1;

        if (ImGui::BeginCombo("Delete Component", deleteCurrentItem >= 0 ? deleteComponents[deleteCurrentItem] : "Select Component to Delete")) {
            for (int i = 0; i < IM_ARRAYSIZE(deleteComponents); i++) {
                bool hasComponent = false;
                switch (i) {
                case 0: hasComponent = global.ecs.has_component<TransformComponent>(selected_entity); break;
                case 1: hasComponent = global.ecs.has_component<LightComponent>(selected_entity); break;
                case 2: hasComponent = global.ecs.has_component<RenderComponent>(selected_entity); break;
                }

                if (hasComponent) {
                    if (ImGui::Selectable(deleteComponents[i], deleteCurrentItem == i)) {
                        switch (i) {
                        case 0: global.ecs.delete_component<TransformComponent>(selected_entity); break;
                        case 1: global.ecs.delete_component<LightComponent>(selected_entity); break;
                        //case 2: global.ecs.delete_component<RenderComponent>(selected_entity); break;
                        }
                        deleteCurrentItem = -1; 
                    }
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    ImGui::Image(m_DeleteIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
    ImGui::SameLine();

    if (ImGui::Button("Delete Entity") /*|| global.input.isKeyPressed(Input::SUPR)*/) {
        
        if (global.ecs.has_component<NameComponent>(selected_entity)) {
            global.ecs.destroy_entity(selected_entity);
        }

    }

    ImGui::End();
}

void UIManager::ContentBrowserWindow() {
    ImGui::Begin("ContentBrowser");

    if (std::filesystem::canonical(m_CurrentDirectory) != std::filesystem::canonical(m_BaseDirectory))
    {
        if (ImGui::ImageButton("button", (ImTextureID)m_BackIcon.getID(), ImVec2(25.0f, 25.0f)))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }

    }

    padding = 16.0f;
    thumbnailSize = 90.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
        const auto& path = directoryEntry.path();
        std::string filenameString = path.filename().string();

        ImGui::PushID(filenameString.c_str());
        Texture icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton("B", (ImTextureID)icon.getID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
        
        if (ImGui::BeginDragDropSource())
        {
            std::filesystem::path relativePath(path);
            const wchar_t* itemPath = relativePath.c_str();
            ImGui::Text("%s", filenameString.c_str());
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (directoryEntry.is_directory())
                m_CurrentDirectory /= path.filename();

        }
        ImGui::TextWrapped(filenameString.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    //ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    //ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}

void UIManager::TopBarWindow() {

    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_MenuBar);

    auto& loc_ecs = global.ecs;
    static int curr_type_draw = 0; 

    if (ImGui::BeginMenuBar()) {

        ImGui::Image(m_SettingsIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        if (ImGui::BeginMenu("Settings")) {
            
            if (ImGui::MenuItem("Toggle Full Screen [F11]")) {
                //global.window->toggleFullScreen(global.input, Input::None, true);
            }

            if (ImGui::MenuItem("Save Scene [CTRL + S]") /*||
                (global.input.isKeyPressed(Input::CTRL) && global.input.isKeyPressed(Input::S))*/) {

                if (global.save_manager.getCurrentScene().empty()) {
                    global.save_manager.saveToFile("../stages/saves/scene_0.json");
                }
                else {
                    global.save_manager.saveToFile(global.save_manager.getCurrentScene());
                }

                AddConsoleMessage("Saving current scene");
            }

            if (ImGui::BeginMenu("Load Scene")) {
                ImGui::Checkbox("Load Scene Window", &load_scene_window);
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("New Scene")) {
                global.save_manager.newFile();
                AddConsoleMessage("New scene created");
            }

            ImGui::EndMenu();
        }

        ImGui::Image(m_EditIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        if (ImGui::BeginMenu("Edit")) {

            if (ImGui::BeginMenu("Render Type")) {
                if (ImGui::MenuItem("Forward Render", nullptr)) {
                    global.deferred_shading = false;
                }
                if (ImGui::MenuItem("Deferred Render", nullptr)) {
                    global.deferred_shading = true;
                }
                ImGui::EndMenu();
            }

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

            if (ImGui::BeginMenu("Console")) {
                ImGui::Checkbox("Console Window", &console_window);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Post Proccess")) {
                ImGui::Checkbox("Post Process Window", &post_process_window);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Fog")) {
                ImGui::Checkbox("Fog Window", &fog_window);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("SkyBox")) {
                ImGui::Checkbox("SkyBox Window", &skybox_window);
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::Image(m_ToolsIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        if (ImGui::BeginMenu("Tools")) {

            if (ImGui::BeginMenu("Add Geometry")) {

                if (ImGui::MenuItem("Sphere")) {
                    global.render_system.createSphere(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Sphere added to scene");
                }

                if (ImGui::MenuItem("Cube 8v")) {
                    global.render_system.createCube8v(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Cube 8v added to scene");
                }

                if (ImGui::MenuItem("Cube 24v")) {
                    global.render_system.createCube24v(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Cube 24v added to scene");
                }

                if (ImGui::MenuItem("Quad")) {
                    global.render_system.createQuad(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Quad added to scene");
                }

                if (ImGui::MenuItem("Terrain")) {
                    global.render_system.createTerrain(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Terrain Light added to scene");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Add Light")) {

                if (ImGui::MenuItem("Point Light")) {
                    global.render_system.createPointLight(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Point Light added to scene");
                }

                if (ImGui::MenuItem("Spot Light")) {
                    global.render_system.createSpotLight(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Spot Light added to scene");
                }

                if (ImGui::MenuItem("Directional Light")) {
                    global.render_system.createDirectionalLight(loc_ecs);
                    global.render_system.init(loc_ecs);
                    AddConsoleMessage("Directional Light added to scene");
                }

                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Add Mesh")) {

                ImGui::Checkbox(("Add mesh Window"), &add_mesh_window);
                
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::Image(m_CameraIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Spacing();
        if (ImGui::BeginMenu("Camera")) {
            
            static glm::vec3 position = global.cam.getPosition();
            if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
                global.cam.setPosition(position);
            }

            static float sensitivity = global.cam.getSensitivity();
            if (ImGui::SliderFloat("Sensitivity", &sensitivity, 0.1f, 1.0f)) {
                global.cam.setSensitibity(sensitivity);
            }

            static float speed = global.cam.getSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 0.001f, 1.0f)) {
                global.cam.setSpeed(speed);
            }

            static float fov = global.cam.getFov();
            if (ImGui::SliderFloat("FOV", &fov, 1.0f, 120.0f)) {
                global.cam.setFov(fov);
            }

            static float near_clip_plane = global.cam.getNearCliPlane();
            if (ImGui::SliderFloat("Near Clip Plane", &near_clip_plane, 0.01f, 10.0f)) {
                global.cam.setNearCliPlane(near_clip_plane);
            }

            static float far_clip_plane = global.cam.getFarCliPlane();
            if (ImGui::SliderFloat("Far Clip Plane", &far_clip_plane, 10.0f, 1000.0f)) {
                global.cam.setFarCliPlane(far_clip_plane);
            }

            static float scroll_speed = global.cam.getScrollSpeed();
            if (ImGui::SliderFloat("Scroll Speed", &scroll_speed, 0.1f, 10.0f)) {
                global.cam.setScrollSpeed(scroll_speed);
            }

            //--

            static bool use_perspective = global.cam.getUsePerspectiveMatrix();
            static bool use_orthographic = !use_perspective; 

            if (ImGui::Checkbox("Perspective Projection", &use_perspective)) {
                if (use_perspective) {
                    use_orthographic = false;
                    global.cam.setUsePerspectiveMatrix(true); 
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Orthographic Projection", &use_orthographic)) {
                if (use_orthographic) {
                    use_perspective = false;
                    global.cam.setUsePerspectiveMatrix(false);
                }
            }

            ImGui::EndMenu();
        }

        ImGui::Image(m_AlgorithmIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Spacing();
        if (ImGui::BeginMenu("Algorithms")) {

            if (ImGui::BeginMenu("LSystem")) {
                ImGui::Checkbox("LSystem Window", &l_system_window);
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }


        ImGui::EndMenuBar();
    }

    ImGui::End();
}
//--

void UIManager::PostProcessWindow()
{
    ImGui::Begin("PostProcess");

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Text("Drag and Drop Vertex and Fragment:");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    static std::string vertex_file_path;
    static std::string vertex_file_name;
    static char* vertex_content = nullptr;
    if (ImGui::Button(!vertex_file_path.empty() ? vertex_file_name.c_str() : ".vs Drop here", { 200.0f, 40.0f })) {}

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            std::filesystem::path filePath = (const wchar_t*)payload->Data;
            if (filePath.extension() == ".vs") {
                vertex_file_path = filePath.string();
                vertex_file_name = filePath.filename().string();
                vertex_content = ReadFile(filePath.string().c_str());
            }
        }
        ImGui::EndDragDropTarget();
    }

    static std::string fragment_file_path;
    static std::string fragment_file_name;
    static char* fragment_content = nullptr;
    if (ImGui::Button(!fragment_file_path.empty() ? fragment_file_name.c_str() : ".fs Drop here", { 200.0f, 40.0f })) {}

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            std::filesystem::path filePath = (const wchar_t*)payload->Data;
            if (filePath.extension() == ".fs") {
                fragment_file_path = filePath.string();
                fragment_file_name = filePath.filename().string();
                fragment_content = ReadFile(filePath.string().c_str());
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    if (ImGui::Button("Compile")) {
        new_pp_shader.compile(vertex_content, fragment_content);
        post_process_compiled = true;

        AddConsoleMessage(vertex_file_name + " and " + fragment_file_name + " compiled");
    }
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    if (post_process_compiled) {
        global.post_process.render(new_pp_shader);

        static bool use_grey = false;
        static bool use_negative = false;
        static bool use_waves = false;
        static bool use_blur = false;
        static bool use_kernel = false;

        ImGui::Checkbox("Grey", &use_grey);
        ImGui::SameLine();
        ImGui::Checkbox("Negative", &use_negative);
        ImGui::SameLine();
        ImGui::Checkbox("Waves", &use_waves);
        ImGui::SameLine();
        ImGui::Checkbox("Blur", &use_blur);
        ImGui::SameLine();
        ImGui::Checkbox("Kernel", &use_kernel);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        if (use_grey) ImGui::DragFloat("Grey##1", &global.post_process.grey_, 0.01f, 0.0f, 1.0f, "%.3f");
        if (use_negative)  ImGui::DragFloat("Negative##1", &global.post_process.negative_, 0.01f, 0.5f, 1.0f, "%.3f");
        if (use_waves)  ImGui::DragFloat("Waves##1", &global.post_process.waves_, 0.001f, 0.00f, 0.09f, "%.3f");
        if (use_blur) ImGui::DragFloat("Blur##1", &global.post_process.blur_, 0.00001f, 0.00f, 0.035f, "%.3f");
        if (use_kernel) ImGui::DragInt("Kernel types##1", &global.post_process.kernel_types_, 0.1f, 0, 2, "%d");

        ImGui::Dummy({ 0.0f, 5.0f });
        ImGui::Separator();
        ImGui::Dummy({ 0.0f, 5.0f });

        if (ImGui::Button("Reset")) {
            post_process_compiled = false;
            use_grey = false;
            use_negative = false;
            use_waves = false;
            use_blur = false;
            use_kernel = false;

            vertex_file_path.clear();
            vertex_file_name.clear();
            fragment_file_path.clear();
            fragment_file_name.clear();

            free(vertex_content);
            free(fragment_content);
        }

        ImGui::Dummy({ 0.0f, 5.0f });
    }

    ImGui::End();
}


void UIManager::FogWindow()
{
    ImGui::Begin("Fog");

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::Checkbox("Activate", &global.use_fog);
    ImGui::Dummy({ 0.0f, 5.0f });

    ImGui::DragFloat("Density", &global.fog_density, 0.001f, 0.0f, 2.0f, "%.3f");
    ImGui::DragFloat("Gradient", &global.fog_gradient, 0.001f, 0.0f, 10.0f, "%.3f");

    ImGui::Dummy({ 0.0f, 5.0f });
    
    float color[3] = {
                    global.sky_colour.r / 255.0f,
                    global.sky_colour.g / 255.0f,
                    global.sky_colour.b / 255.0f
    };

    if (ImGui::ColorPicker3("Sky Colour", color)) {
        global.sky_colour.x = color[0];
        global.sky_colour.y = color[1];
        global.sky_colour.z = color[2];
    }


    ImGui::End();
}

void UIManager::SkyboxWindow()
{
    ImGui::Begin("SkyBox");

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::Checkbox("Activate", &global.use_skybox);
    ImGui::SameLine();
    ImGui::Checkbox("Use Cubemap / DDS", &global.skybox.use_cubemap);
    ImGui::Dummy({ 0.0f, 5.0f });

    ImGui::Separator();

    ImGui::Dummy({ 0.0f, 5.0f });
    static bool temp_sb = global.skybox.use_cubemap;
    ImGui::Checkbox("Compile Cubemap / DDS", &temp_sb);
    ImGui::Dummy({ 0.0f, 5.0f });

    ImGui::Separator();
    ImGui::Dummy({ 0.0f, 5.0f });

    if (temp_sb) {
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Drag and Drop 6 Cubemap files");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Use this order: right, left, bottom, top, front, back");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        static std::vector<std::string> cubemap_file_paths(6);
        static std::vector<std::string> cubemap_file_names(6);

        for (int i = 0; i < 6; i++) {
            ImGui::PushID(i);
            if (ImGui::Button(!cubemap_file_paths[i].empty() ? cubemap_file_names[i].c_str() : "Cubemap file", { 200.0f, 40.0f })) {}

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    std::filesystem::path filePath = (const wchar_t*)payload->Data;
                    if (filePath.extension() == ".png" || filePath.extension() == ".jpg") {
                        cubemap_file_paths[i] = filePath.string();
                        cubemap_file_names[i] = filePath.filename().string();
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::PopID();
        }

        ImGui::Dummy(ImVec2(0.0f, 3.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        if (ImGui::Button("Compile Cubemap")) {
            if (std::all_of(cubemap_file_paths.begin(), cubemap_file_paths.end(), [](const std::string& s) { return !s.empty(); })) {
                std::vector<std::string> full_paths;
                for (const auto& file : cubemap_file_paths) {
                    full_paths.push_back(file);
                }
                global.skybox.loadCubeMap(
                    full_paths,
                    "../stages/shaders/skybox/skybox_cubemap.vs",
                    "../stages/shaders/skybox/skybox_cubemap.fs"
                );
                global.skybox.initcubeMap();
                AddConsoleMessage("Cubemap compiled");

                //cubemap_file_paths.clear();
                //cubemap_file_names.clear();
            }
        }
    }
    else {
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Drag and Drop .dds file");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        static std::string skybox_file_path;
        static std::string skybox_file_name;
        if (ImGui::Button(!skybox_file_path.empty() ? skybox_file_name.c_str() : ".dds Drop here", { 200.0f, 40.0f })) {}

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::filesystem::path filePath = (const wchar_t*)payload->Data;
                if (filePath.extension() == ".dds") {
                    skybox_file_path = filePath.string();
                    skybox_file_name = filePath.filename().string();
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::Dummy(ImVec2(0.0f, 3.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        if (ImGui::Button("Compile DDS")) {
            global.skybox.loadDDS(
                (skybox_file_path).c_str(),
                "../stages/shaders/skybox/skybox_dds.vs",
                "../stages/shaders/skybox/skybox_dds.fs"
            );
            global.skybox.initDDS();

            AddConsoleMessage(skybox_file_name + " compiled");

            skybox_file_path.clear();
            skybox_file_name.clear();
        }
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }
    
    

    ImGui::End();
}

void UIManager::ConsoleWindow()
{
    ImGui::Begin("Console");

    const float footer_height = ImGui::GetFrameHeightWithSpacing() + 5.0f;
    ImGui::BeginChild("ConsoleOutput", ImVec2(0, -footer_height), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& message : consoleMessages)
    {
        ImGui::TextUnformatted(message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    static char inputBuf[256] = "";
    ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputFlags))
    {
        if (inputBuf[0] != '\0')
        {
            AddConsoleMessage(inputBuf);
        }
        inputBuf[0] = '\0'; 
        ImGui::SetKeyboardFocusHere(-1); 
    }

    ImGui::End();
}


void UIManager::AddConsoleMessage(const std::string& message)
{
    consoleMessages.push_back(message);

    if (consoleMessages.size() > 1000)
    {
        consoleMessages.erase(consoleMessages.begin()); 
    }
}

void UIManager::LoadSceneWindow()
{
    ImGui::Begin("Load Scene");

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Text("Drag and Drop json file:");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    static std::string scene_file_path;
    static std::string scene_file_name;
    std::filesystem::path filePath;
    if (ImGui::Button(!scene_file_path.empty() ? scene_file_name.c_str() : ".json Drop here", { 200.0f, 40.0f })) {}

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            filePath = (const wchar_t*)payload->Data;
            if (filePath.extension() == ".json") {
                scene_file_path = filePath.string();
                scene_file_name = filePath.filename().string();
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Dummy(ImVec2(0.0f, 3.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 3.0f));

    if (ImGui::Button("Load Scene")) {
        global.save_manager.setCurrentScene(scene_file_path);
        global.save_manager.loadFromFile(global.save_manager.getCurrentScene());

        AddConsoleMessage(scene_file_path + " loaded");

        scene_file_path.clear();
        scene_file_name.clear();
    }

    ImGui::End();
}

void UIManager::AddMeshWindow()
{
    ImGui::Begin("Add Mesh");

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Text("Drag and Drop obj file:");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    static std::string obj_file_path;
    static std::string obj_file_name;
    static std::string texture_folder_path;
    static std::string texture_folder_name;
    if (ImGui::Button(!obj_file_path.empty() ? obj_file_name.c_str() : ".obj Drop here", { 200.0f, 40.0f })) {}

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            std::filesystem::path filePath = (const wchar_t*)payload->Data;
            if (filePath.extension() == ".obj") {
                obj_file_path = filePath.string();
                obj_file_name = filePath.filename().string();
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::Button(!texture_folder_path.empty() ? texture_folder_name.c_str() : "Texture Folder Drop here", { 200.0f, 40.0f })) {}

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            std::filesystem::path filePath = (const wchar_t*)payload->Data;
            if (std::filesystem::is_directory(filePath)) {
                texture_folder_path = filePath.string();
                texture_folder_name = filePath.filename().string();
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Dummy(ImVec2(0.0f, 3.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 3.0f));

    if (ImGui::Button("Add Mesh")) {
        
        if (!obj_file_path.empty() && !texture_folder_path.empty()) {
            global.render_system.createMesh(global.ecs, obj_file_path, texture_folder_path);
            global.render_system.init(global.ecs);

            AddConsoleMessage(obj_file_name + " added to scene");

            obj_file_path.clear();
            obj_file_name.clear();
            texture_folder_path.clear();
            texture_folder_name.clear();
        }
    }

    ImGui::End();
}

//--

void UIManager::Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity) {

    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    auto& loc_ecs = global.ecs;

    ImGuizmo::BeginFrame();
    if (ImGui::Begin("GameWindow", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        auto ti = loc_ecs.getComponentList<TransformComponent>().begin();
        auto li = loc_ecs.getComponentList<LightComponent>().begin();
        auto tiend = loc_ecs.getComponentList<TransformComponent>().end();
        auto liend = loc_ecs.getComponentList<LightComponent>().end();

        int entity_id = 0;

        for (; ti != tiend; ++ti, ++entity_id) {
            
            if (entity_id == selected_entity && ti->has_value()) {

                auto& transform = loc_ecs.get_component<TransformComponent>(selected_entity);

                if (loc_ecs.has_component<LightComponent>(selected_entity)) {
                    auto& light_comp = loc_ecs.get_component<LightComponent>(selected_entity);
                    light_comp->light.pos_ = transform->position;
                    light_comp->light.dir_ = glm::mat3(transform->rotation) * glm::vec3(0.0f, 0.0f, 1.0f);
                }

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

    if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        if (selected_entity >= 0) {
            ImGui::Dummy({ 0.0f, 5.0f });
            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;
            ImGui::Dummy({ 0.0f, 5.0f });
        }
    }
    ImGui::End();
}

void UIManager::LSystemWindow() {
    ImGui::Begin("L-System");

    static std::string axiom = "F";
    static std::string rule = "F=FF+[+F-F-F]-[-F+F+F]";
    static int iterations = 3; 

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::InputText("Axiom", &axiom);
    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::InputText("Rule", &rule);
    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::InputInt("Iterations", &iterations);

    static std::map<char, std::string> rule_map;
    size_t equals_pos = rule.find('=');
    if (equals_pos != std::string::npos) {
        char key = rule[0];
        std::string value = rule.substr(equals_pos + 1);
        rule_map[key] = value;
    }

    static float angle = glm::radians(30.0f);
    static glm::vec3 cube_size = { 0.4f, 2.0f, 0.4f };
    static float spacing = 2.0f;
    static glm::vec3 starting_pos = { 0.0f, 0.0f, 0.0f };
    static glm::quat starting_rot = { 1.0f, 0.0f, 0.0f, 0.0f };

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::SliderAngle("Angle", &angle);
    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::DragFloat3("Cube8v size", &cube_size.x);
    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::InputFloat("Spacing", &spacing);
    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::DragFloat3("Starting position", &starting_pos.x);
    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::DragFloat4("Starting rotation", glm::value_ptr(starting_rot), 0.01f, -1.0f, 1.0f);

    ImGui::Dummy({ 0.0f, 5.0f });

    ImGui::Separator();
    ImGui::Dummy({ 0.0f, 2.0f });

    if (ImGui::Button("Start L-System algorithm")) {
        LSystem lsystem(global.ecs, global.render_system);
        lsystem.angle_ = angle;
        lsystem.cube_size_ = cube_size;
        lsystem.spacing_ = spacing;
        lsystem.starting_pos_ = starting_pos;
        lsystem.starting_rot_ = glm::normalize(starting_rot);

        lsystem.init(axiom, rule_map, iterations);
    }

    ImGui::Dummy({ 0.0f, 5.0f });

    if (ImGui::Button("Delete all entities")) {
        global.ecs.remove_entities();
    }

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::End();
}