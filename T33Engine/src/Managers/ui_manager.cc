
#include "Managers/ui_manager.h"
#include "Managers/engine.h"
#include "utils.h"
#include "Render/post_processing.h"
#include "Components/components.h"

#include "Render/skybox.h"
#include "Algorithms/l_system.h"

#include "Managers/save_manager.h"
#include "Core/window.h"

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

UIManager::UIManager(Engine& engine, ECSManager& ecs, RenderSystem& rs)
    : selected_entity(0),
    m_BaseDirectory("../"),
    m_CurrentDirectory(m_BaseDirectory),
    padding(16.0f),
    thumbnailSize(128.0f),
    post_process_window(false),
    add_entities_terrain_window(false),
    add_surface_window(false),
    console_window(false),
    fog_window(false),
    l_system_window(false),
    new_pp_shader(),
    m_PuzzleIcon(),
    m_DeleteIcon(),
    post_process_compiled(false),
    add_mesh_window(false),
    show_fps(false),
    window_(),
    ecs_(ecs),
    rs_(rs),
    engine_(engine),
    input_(nullptr)
{
}

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

void UIManager::Init(Window* window, Input* input) {
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
    m_PhysicsmIcon.loadTexture("../stages/assets/textures/imgui/physics.png");
    m_SkyIcon.loadTexture("../stages/assets/textures/imgui/sky.png");
    m_LuaIcon.loadTexture("../stages/assets/textures/imgui/lua.png");
    //---

    input_ = input;
    window_ = window;

    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

void UIManager::Render() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImguiWindow();

    if (!engine_.is_playing) {
        HierarchyWindow();
        InspectorWindow();
        ContentBrowserWindow();
        TopBarWindow();

        if (console_window) ConsoleWindow();
        if (fog_window) FogWindow();
        if (post_process_window) PostProcessWindow();
        if (add_mesh_window) AddMeshWindow();
        if (add_surface_window) AddSurfaceWindow();
        if (l_system_window) LSystemWindow();
        if (add_entities_terrain_window) AddEntitiesTerrainWindow();
    }

    engine_.post_process.endRender();

    GameWindow();

    // ---

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
void UIManager::ImguiWindow() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    if (engine_.is_playing) {
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
        ImGui::DockBuilderDockWindow("Add Surface", dock_right);
        ImGui::DockBuilderDockWindow("L-System", dock_right); 
        ImGui::DockBuilderDockWindow("Spawn Entities", dock_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

void UIManager::GameWindow() {
    ImVec2 button_size = { 20.0f, 20.0f };
    ImVec2 button_pos;

    if (engine_.is_playing) {
        ImGui::Begin("Main", nullptr);
    }
    else {
        ImGui::Begin("GameWindow", nullptr);
    }

    ImVec2 window_size = ImGui::GetContentRegionAvail();

    if (engine_.is_playing) {
        button_pos = { (window_size.x - button_size.x) * 0.5f, 0.0f };
    }
    else {
        button_pos = { (window_size.x - button_size.x) * 0.5f, 25.0f };
    }

    ImGui::SetCursorPos(button_pos);

    ImTextureID icon = engine_.is_playing ? m_StopIcon.getID() : m_PlayIcon.getID();
    if (ImGui::ImageButton("Play", icon, button_size, { 0, 1 }, { 1, 0 })) {
        engine_.is_playing = !engine_.is_playing;
        rs_.is_playing = engine_.is_playing;
    }

    GLuint captureTexture = engine_.post_process.frame_buffer_.GetColorTexture();
    if (captureTexture) ImGui::Image((ImTextureID)(intptr_t)captureTexture, { window_size.x, window_size.y - (button_size.y + 7.0f)}, { 0.0f, 1.0f }, { 1.0f, 0.0f });

    ImGui::End();
}

void UIManager::HierarchyWindow() {
    ImGui::Begin("Hierarchy", nullptr);

    auto ni = ecs_.getComponentList<IdentifierComponent>().begin();
    auto niend = ecs_.getComponentList<IdentifierComponent>().end();

    auto hi = ecs_.getComponentList<HierarchyComponent>().begin();
    auto hiend = ecs_.getComponentList<HierarchyComponent>().end();

    auto ti = ecs_.getComponentList<TransformComponent>().begin();
    auto tiend = ecs_.getComponentList<TransformComponent>().end();

    int entity_id = 0;

    ImGui::Selectable("Root", false, ImGuiSelectableFlags_None, ImVec2(0, 20));
    if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY");
        if (payload) {
            int draggedEntityId = *reinterpret_cast<int*>(payload->Data);
            auto& draggedHierarchy = ecs_.get_component<HierarchyComponent>(draggedEntityId);

            if (!draggedHierarchy->getParents().empty()) {
                auto previousParentId = draggedHierarchy->getParent();
                auto& previousParentHierarchy = ecs_.get_component<HierarchyComponent>(previousParentId);
                previousParentHierarchy->removeChild(draggedEntityId);
                draggedHierarchy->clearParents();
            }
        }
        ImGui::EndDragDropTarget();
    }

    for (; ni != niend && hi != hiend; ++ni, ++hi, entity_id++) {

        if ((ni->has_value() && hi->has_value())) {
            auto& name_comp = *ni;
            auto& hierarchy_comp = *hi;

            if (hierarchy_comp->getParents().empty()) {
                HierarchyTree(name_comp->name, entity_id);
            }

            if (ti != tiend && ti->has_value()) {
                auto& transf_comp = *ti;

                if (hierarchy_comp->hasParent()) {
                    auto parent_id = hierarchy_comp->getParent();

                    if (ecs_.has_component<TransformComponent>(parent_id)) {
                        auto& child_transform = ecs_.get_component<TransformComponent>(entity_id);
                        auto& parent_transform = ecs_.get_component<TransformComponent>(parent_id);

                        if (selected_entity == entity_id) {
                            child_transform->local_position = child_transform->position - parent_transform->position;
                        }
                        else {
                            child_transform->position = parent_transform->position + child_transform->local_position;
                        }
                    }
                    else {
                        auto& child_transform = ecs_.get_component<TransformComponent>(entity_id);
                        child_transform->position = child_transform->local_position;
                    }
                }

                ++ti;
            }
        }
    }

    ImGui::End();
}

void UIManager::HierarchyTree(const std::string& name, int entity_id)
{
    ImGui::Image(m_EntityIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
    ImGui::SameLine();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);

    ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (selected_entity == entity_id)
        flag |= ImGuiTreeNodeFlags_Selected;

    if (ImGui::TreeNodeEx(name.c_str(), flag))
    {
        if (ImGui::IsItemClicked()) {
            selected_entity = entity_id;
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("HIERARCHY", &entity_id, sizeof(int));
            ImGui::Text(name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY");
            if (payload) {
                int draggedEntityId = *reinterpret_cast<int*>(payload->Data);

                auto& draggedHierarchy = ecs_.get_component<HierarchyComponent>(draggedEntityId);
                auto& targetHierarchy = ecs_.get_component<HierarchyComponent>(entity_id);

                if (!draggedHierarchy->hasChildren()) {

                    if (!draggedHierarchy->getParents().empty()) {
                        auto previousParentId = draggedHierarchy->getParent();
                        auto& previousParentHierarchy = ecs_.get_component<HierarchyComponent>(previousParentId);
                        if (!draggedHierarchy->hasParent()) {
                            previousParentHierarchy->removeChild(draggedEntityId);
                        }
                    }

                    if (!draggedHierarchy->hasParent()) {
                        targetHierarchy->addChild(draggedEntityId);
                        draggedHierarchy->clearParents();
                        draggedHierarchy->addParent(entity_id);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        auto& hierarchy = ecs_.get_component<HierarchyComponent>(entity_id);
        for (auto childId : hierarchy->getChildren()) {
            auto& childNameComp = ecs_.get_component<IdentifierComponent>(childId);
            if (childNameComp.has_value()) {
                HierarchyTree(childNameComp->name, childId);
            }
        }

        ImGui::TreePop();
    }
}

void UIManager::InspectorWindow() {
    ImGui::Begin("Inspector");
 
    if (ecs_.has_component<IdentifierComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##IdentifierComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_TagIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Identifier Component");

        if (open) {
            auto& name_comp = ecs_.get_component<IdentifierComponent>(selected_entity);
            ImGui::Dummy({ 0.0f, 3.0f });

            ImGui::Text("ID: %d", name_comp->id);

            if (name_comp->name.empty()) name_comp->name = "Entity";

            if (ImGui::InputText("##NameInput", &name_comp->name)) {
                if (name_comp->name.empty()) name_comp->name = "Entity";
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

    }

    if (ecs_.has_component<CameraComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##CameraComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_CameraIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Camera Component");

        if (open) {
            
            auto& camera_comp = ecs_.get_component<CameraComponent>(selected_entity);

            ImGui::Dummy({0.0f, 5.0f });

            glm::vec3 position = camera_comp->camera.getPosition();
            if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
                camera_comp->camera.setPosition(position);
            }

            float sensitivity = camera_comp->camera.getSensitivity();
            if (ImGui::SliderFloat("Sensitivity", &sensitivity, 0.1f, 1.0f)) {
                camera_comp->camera.setSensitibity(sensitivity);
            }

            float speed = camera_comp->camera.getSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 1.0f, 50.0f)) {
                camera_comp->camera.setSpeed(speed);
            }

            float fov = camera_comp->camera.getFov();
            if (ImGui::SliderFloat("FOV", &fov, 1.0f, 120.0f)) {
                camera_comp->camera.setFov(fov);
            }

            float near_clip_plane = camera_comp->camera.getNearCliPlane();
            if (ImGui::SliderFloat("Near Clip Plane", &near_clip_plane, 0.01f, 10.0f)) {
                camera_comp->camera.setNearCliPlane(near_clip_plane);
            }

            float far_clip_plane = camera_comp->camera.getFarCliPlane();
            if (ImGui::SliderFloat("Far Clip Plane", &far_clip_plane, 10.0f, 1000.0f)) {
                camera_comp->camera.setFarCliPlane(far_clip_plane);
            }

            float scroll_speed = camera_comp->camera.getScrollSpeed();
            if (ImGui::SliderFloat("Scroll Speed", &scroll_speed, 0.1f, 10.0f)) {
                camera_comp->camera.setScrollSpeed(scroll_speed);
            }

            //--
            ImGui::Dummy({ 0.0f, 2.0f });

            bool use_perspective = camera_comp->camera.getUsePerspectiveMatrix();
            bool use_orthographic = !use_perspective;

            if (ImGui::Checkbox("Perspective Projection", &use_perspective)) {
                if (use_perspective) {
                    use_orthographic = false;
                    camera_comp->camera.setUsePerspectiveMatrix(true);
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Orthographic Projection", &use_orthographic)) {
                if (use_orthographic) {
                    use_perspective = false;
                    camera_comp->camera.setUsePerspectiveMatrix(false);
                }
            }

            ImGui::Dummy({ 0.0f, 5.0f });
        }
    }

    if (ecs_.has_component<SkyboxComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##SkyboxComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_SkyIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Skybox Component");

        if (open) {
            auto& skybox_comp = ecs_.get_component<SkyboxComponent>(selected_entity);

            ImGui::Dummy({ 0.0f, 5.0f });
            ImGui::Checkbox("Activate", &rs_.use_skybox);
            ImGui::SameLine();
            ImGui::Checkbox("Use Cubemap / DDS", &skybox_comp->skybox.use_cubemap);
            ImGui::Dummy({ 0.0f, 5.0f });

            ImGui::Separator();

            ImGui::Dummy({ 0.0f, 5.0f });
            static bool temp_sb = skybox_comp->skybox.use_cubemap;
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
                        skybox_comp->skybox.loadCubeMap(
                            full_paths,
                            "../stages/shaders/skybox/skybox_cubemap.vs",
                            "../stages/shaders/skybox/skybox_cubemap.fs"
                        );
                        skybox_comp->skybox.initcubeMap();
                        AddConsoleMessage("Cubemap compiled");

                        //cubemap_file_paths.clear();
                        //cubemap_file_names.clear();
                    }
                }
                ImGui::Dummy({ 0.0f, 5.0f });
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
                    skybox_comp->skybox.loadDDS(
                        (skybox_file_path).c_str(),
                        "../stages/shaders/skybox/skybox_dds.vs",
                        "../stages/shaders/skybox/skybox_dds.fs"
                    );
                    skybox_comp->skybox.initDDS();

                    AddConsoleMessage(skybox_file_name + " compiled");

                    skybox_file_path.clear();
                    skybox_file_name.clear();
                }
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
            }
        }
    }

    if (ecs_.has_component<TransformComponent>(selected_entity)) {

            bool open = ImGui::CollapsingHeader("##TransformComponent", ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::SameLine();
            ImGui::Image(m_TransformIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            ImGui::SameLine();
            ImGui::Text("Transform Component");

            if (open) {
                auto& transf_comp = ecs_.get_component<TransformComponent>(selected_entity);
                auto ci = ecs_.getComponentList<CameraComponent>().begin();
                auto ciend = ecs_.getComponentList<CameraComponent>().end();

                for (; ci != ciend; ++ci) {
                    if (ci->has_value()) {
                        auto& camera_comp = *ci;
                        if (camera_comp->camera.getUsePerspectiveMatrix()) {
                            ImGuizmo::SetOrthographic(false);
                            Gizmo(camera_comp->camera.getViewMatrix(), camera_comp->camera.getPerspectiveMatrix(), selected_entity);
                        }
                        else {
                            ImGuizmo::SetOrthographic(true);
                            Gizmo(camera_comp->camera.getViewMatrix(), camera_comp->camera.getOrthograficMatrix(), selected_entity);
                        }
                    }
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

    if (ecs_.has_component<RenderComponent>(selected_entity)) {

            auto& render_comp = ecs_.get_component<RenderComponent>(selected_entity);

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

                    initShader(rs_.shader, vertex_file_path, fragment_file_path);

                    vertex_file_path.clear();
                    vertex_file_name.clear();
                    fragment_file_path.clear();
                    fragment_file_name.clear();
                }
                ImGui::Dummy(ImVec2(0.0f, 5.0f));

                // -----------------------------------------------------------------

                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));

                switch (render_comp->type_) {
                case RenderType::RT_Mesh:
                    ImGui::Checkbox("Use Texture", &render_comp->mesh_->use_texture_);
                    break;
                case RenderType::RT_Terrain:
                    ImGui::Checkbox("Use Texture", &render_comp->terrain_->use_texture_);
                    break;
                case RenderType::RT_Water:
                    ImGui::Checkbox("Use Texture", &render_comp->water_->use_texture_);
                    break;
                case RenderType::RT_Sphere:
                    ImGui::Checkbox("Use Texture", &render_comp->sphere_->use_texture_);
                    break;
                case RenderType::RT_Surface:
                    ImGui::Checkbox("Use Texture", &render_comp->surface_->use_texture_);
                    break;
                case RenderType::RT_Quad:
                    ImGui::Checkbox("Use Texture", &render_comp->quad_->use_texture_);
                    break;
                case RenderType::RT_Cube8v:
                case RenderType::RT_Cube24v:
                    ImGui::Checkbox("Use Texture", &render_comp->cube_->use_texture_);
                    break;
                }

                ImGui::Dummy(ImVec2(0.0f, 5.0f));

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.3f, 0.0f, 1.0f));
                ImGui::Text("Change Texture");
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::PopStyleColor();

                static std::string texture_file_path;
                static std::string texture_file_name;
                if (ImGui::Button(!texture_file_path.empty() ? texture_file_name.c_str() : ".png/.jpg drop texture here", { 200.0f, 40.0f })) {}

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
                    case RenderType::RT_Water:
                        render_comp->water_->setTexture(texture_file_path);
                        break;
                    case RenderType::RT_Sphere:
                        render_comp->sphere_->setTexture(texture_file_path);
                        break;
                    case RenderType::RT_Surface:
                        render_comp->surface_->setTexture(texture_file_path);
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

                if (render_comp->type_ == RenderType::RT_Quad) {

                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    static bool use_normal_map_global = false;
                    if (ImGui::Checkbox("Use Normal map", &use_normal_map_global)) {
                        auto ri = ecs_.getComponentList<RenderComponent>().begin();
                        auto riend = ecs_.getComponentList<RenderComponent>().end();

                        for (; ri != riend; ++ri) {
                            if (ri->has_value()) {
                                auto& loc_render_comp = *ri;
                                if (loc_render_comp->type_ == RenderType::RT_Quad)
                                    loc_render_comp->quad_->use_normal_map_ = use_normal_map_global;
                            }
                        }
                    }
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.5f, 1.0f, 1.0f));
                    ImGui::Text("Change/Add Normal map");
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PopStyleColor();

                    static std::string normalmap_file_path;
                    static std::string normalmap_file_name;

                    if (ImGui::Button(!normalmap_file_path.empty() ? normalmap_file_name.c_str() : ".png/.jpg drop normal here", { 200.0f, 40.0f })) {}

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            std::filesystem::path filePath = (const wchar_t*)payload->Data;
                            if ((filePath.extension() == ".png") || (filePath.extension() == ".jpg")) {
                                normalmap_file_path = filePath.string();
                                normalmap_file_name = filePath.filename().string();
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));

                    if (ImGui::Button("Compile Normal map")) {
                        render_comp->quad_->setNormalMap(normalmap_file_path);

                        normalmap_file_path.clear();
                        normalmap_file_name.clear();
                    }

                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    //---------------

                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    static bool use_parallax_map_global = false;
                    ImGui::Checkbox("Use Parallax map", &use_parallax_map_global);

                    if (ImGui::IsItemEdited()) {
                        auto ri = ecs_.getComponentList<RenderComponent>().begin();
                        auto riend = ecs_.getComponentList<RenderComponent>().end();

                        for (; ri != riend; ++ri) {
                            if (ri->has_value()) {
                                auto& loc_render_comp = *ri;
                                if (loc_render_comp->type_ == RenderType::RT_Quad && loc_render_comp->quad_) {
                                    loc_render_comp->quad_->use_parallax_map_ = use_parallax_map_global;
                                }
                            }
                        }
                    }

                    static float global_parallax_scale = 0.05f;
                    ImGui::DragFloat("Parallax Scale", &global_parallax_scale, 0.01f, 0.0f, 1.0f);

                    static int global_parallax_type = 0;
                    const char* parallaxTypes[] = { "Basic Parallax", "Steep Parallax", "Parallax Occlusion" };
                    global_parallax_type = std::clamp(global_parallax_type, 0, IM_ARRAYSIZE(parallaxTypes) - 1);

                    const char* currentType = parallaxTypes[global_parallax_type];

                    if (ImGui::BeginCombo("Parallax Type", currentType)) {
                        for (int n = 0; n < IM_ARRAYSIZE(parallaxTypes); ++n) {
                            bool is_selected = (global_parallax_type == n);
                            if (ImGui::Selectable(parallaxTypes[n], is_selected)) {
                                global_parallax_type = n;
                            }
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    auto ri = ecs_.getComponentList<RenderComponent>().begin();
                    auto riend = ecs_.getComponentList<RenderComponent>().end();

                    for (; ri != riend; ++ri) {
                        if (ri->has_value()) {
                            auto& loc_render_comp = *ri;
                            if (loc_render_comp->type_ == RenderType::RT_Quad && loc_render_comp->quad_) {
                                loc_render_comp->quad_->parallax_scale_ = global_parallax_scale;
                                loc_render_comp->quad_->parallax_type = static_cast<ParallaxType>(global_parallax_type);
                            }
                        }
                    }

                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.40f, 0.40f, 1.0f));
                    ImGui::Text("Change/Add Parallax map");
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PopStyleColor();

                    static std::string parallaxmap_file_path;
                    static std::string parallaxmap_file_name;

                    if (ImGui::Button(!parallaxmap_file_path.empty() ? parallaxmap_file_name.c_str() : ".png/.jpg drop parallax here", { 200.0f, 40.0f })) {}

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            std::filesystem::path filePath = (const wchar_t*)payload->Data;
                            if ((filePath.extension() == ".png") || (filePath.extension() == ".jpg")) {
                                parallaxmap_file_path = filePath.string();
                                parallaxmap_file_name = filePath.filename().string();
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));

                    if (ImGui::Button("Compile Parallax map")) {
                        render_comp->quad_->setParallaxMap(parallaxmap_file_path);

                        parallaxmap_file_path.clear();
                        parallaxmap_file_name.clear();
                    }
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

                    auto& terrain = *ecs_.get_component<RenderComponent>(selected_entity)->terrain_;

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
                case ::RT_Water:
                {
                    ImGui::Separator();

                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::SliderFloat("Alpha", &rs_.alpha_blend, 0.0f, 1.0f, "%0.03f");
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::Text("Water Dimensions");
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& water = *ecs_.get_component<RenderComponent>(selected_entity)->water_;

                    static int temp_num_cols = water.num_cols_;
                    static int temp_num_rows = water.num_rows_;
                    static float temp_size = water.size_;
                    static bool temp_is_centered = water.is_centered_;

                    ImGui::InputInt("Number of Columns", &temp_num_cols);
                    ImGui::InputInt("Number of Rows", &temp_num_rows);
                    ImGui::SliderFloat("Size", &temp_size, 0.0f, 10.0f, "%.3f");
                    ImGui::Checkbox("Is Centered", &temp_is_centered);

                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    if (ImGui::Button("Apply Changes")) {

                        water.init(temp_num_cols, temp_num_rows, 1.0f,
                            temp_size, temp_is_centered, true);

                        water.bind();
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

                    auto& sphere = *ecs_.get_component<RenderComponent>(selected_entity)->sphere_;

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

                    auto& cube = *ecs_.get_component<RenderComponent>(selected_entity)->cube_;
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

                    auto& cube = *ecs_.get_component<RenderComponent>(selected_entity)->cube_;
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

                    auto& quad = *ecs_.get_component<RenderComponent>(selected_entity)->quad_;

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

                    auto& terrain = *ecs_.get_component<RenderComponent>(selected_entity)->terrain_;
                    ImGui::Checkbox("Use Wave Effect", &terrain.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &terrain.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &terrain.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &terrain.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &terrain.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &terrain.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                case ::RT_Water:
                {
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                    ImGui::Text("Wave Options");
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& water = *ecs_.get_component<RenderComponent>(selected_entity)->water_;
                    ImGui::Checkbox("Use Wave Effect", &water.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &water.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &water.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &water.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &water.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &water.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                case ::RT_Sphere:
                {
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                    ImGui::Text("Wave Options");
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& sphere = *ecs_.get_component<RenderComponent>(selected_entity)->sphere_;
                    ImGui::Checkbox("Use Wave Effect", &sphere.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &sphere.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &sphere.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &sphere.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &sphere.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &sphere.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                case ::RT_Surface:
                {
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                    ImGui::Text("Wave Options");
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& surface = *ecs_.get_component<RenderComponent>(selected_entity)->surface_;
                    ImGui::Checkbox("Use Wave Effect", &surface.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &surface.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &surface.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &surface.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &surface.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &surface.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                case ::RT_Quad:
                {
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                    ImGui::Text("Wave Options");
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& quad = *ecs_.get_component<RenderComponent>(selected_entity)->quad_;
                    ImGui::Checkbox("Use Wave Effect", &quad.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &quad.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &quad.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &quad.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &quad.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &quad.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                case ::RT_Mesh:
                {
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                    ImGui::Text("Wave Options");
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& mesh = *ecs_.get_component<RenderComponent>(selected_entity)->mesh_;
                    ImGui::Checkbox("Use Wave Effect", &mesh.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &mesh.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &mesh.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &mesh.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &mesh.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &mesh.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                case ::RT_Cube8v:
                case ::RT_Cube24v:
                {
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.378f, 0.547f, 0.902f, 1.0f));
                    ImGui::Text("Wave Options");
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));

                    auto& cube = *ecs_.get_component<RenderComponent>(selected_entity)->cube_;
                    ImGui::Checkbox("Use Wave Effect", &cube.wave_.use_wave_);

                    ImGui::SliderFloat("Amplitude", &cube.wave_.amplitude_, 0.0f, 1.0f);
                    ImGui::SliderFloat("Length", &cube.wave_.wave_length_, 0.1f, 10.0f);
                    ImGui::SliderFloat("Speed", &cube.wave_.speed_, 0.1f, 5.0f);

                    ImGui::SliderFloat("Direction X", &cube.wave_.direction_.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &cube.wave_.direction_.y, -1.0f, 1.0f);
                    ImGui::Dummy(ImVec2(0.0f, 2.0f));
                }
                break;
                }
            }
        }

    if (ecs_.has_component<LightComponent>(selected_entity)) {

            bool open = ImGui::CollapsingHeader("##LightComponent", ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::SameLine();
            ImGui::Image(m_LightIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            ImGui::SameLine();
            ImGui::Text("Light Component");

            if (open) {
                auto& light_comp = ecs_.get_component<LightComponent>(selected_entity);

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

                    light_comp->shadow_map_.init2D();
                }
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
            }
        }

    if (ecs_.has_component<PhysicsComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##PhysicsComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_PhysicsmIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Physics Component");

        if (open) {

            auto& physics = *ecs_.get_component<PhysicsComponent>(selected_entity);
            
            ImGui::Text("Gravity");
            ImGui::SliderFloat("Y##Gravity", &physics.gravity.y, -100.0f, 100.0f);

            ImGui::Spacing();
            ImGui::InputFloat("Mass##Physics", &physics.mass, 0.1f, 1.0f, "%.2f");
            ImGui::InputFloat("Floor Height", &physics.floor_height, 0.1f, 1.0f, "%.2f");
            ImGui::SliderFloat("Bounce Factor", &physics.bounce_factor, 0.0f, 1.0f);
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    if (ecs_.has_component<ScriptingComponent>(selected_entity)) {

        bool open = ImGui::CollapsingHeader("##ScriptingComponent", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_LuaIcon.getID(), {20.0f, 20.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
        ImGui::SameLine();
        ImGui::Text("Scripting Component");

        if (open) {

            auto& ls_comp = ecs_.get_component<ScriptingComponent>(selected_entity);

            static std::string lua_file_path;
            static std::string lua_file_name;

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (ImGui::Button(!lua_file_path.empty() ? lua_file_name.c_str() : ".lua drop here", { 200.0f, 40.0f })) {}

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    std::filesystem::path filePath = (const wchar_t*)payload->Data;
                    if (filePath.extension() == ".lua") {
                        lua_file_path = filePath.string();
                        lua_file_name = filePath.filename().string();
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            if (ImGui::Button("Read file")) {

                ls_comp.value().script_path = lua_file_path;
                ls_comp.value().entity_id = selected_entity;
                engine_.scripting_system.init();
            }
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }

    // -------

    static bool has_any_component = false;

    if (ecs_.has_component<IdentifierComponent>(selected_entity) ||
        ecs_.has_component<HierarchyComponent>(selected_entity) ||
        ecs_.has_component<RenderComponent>(selected_entity) ||
        ecs_.has_component<TransformComponent>(selected_entity) ||
        ecs_.has_component<LightComponent>(selected_entity) ||
        ecs_.has_component<PhysicsComponent>(selected_entity) ||
        ecs_.has_component<CameraComponent>(selected_entity) ||
        ecs_.has_component<SkyboxComponent>(selected_entity)
        ) {
        has_any_component = true;
    }
    else {
        has_any_component = false;
    }

    if (has_any_component) {
        bool open = ImGui::CollapsingHeader("##Components", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::SameLine();
        ImGui::Image(m_PuzzleIcon.getID(), { 20.0f, 20.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        ImGui::Text("Components");

        if (open) {
            ImGui::Dummy(ImVec2(0.0f, 3.0f));

            ImGui::Text("Entity has:");
            ImGui::Dummy(ImVec2(0.0f, 3.0f));

            if (ecs_.has_component<TransformComponent>(selected_entity)) {
                ImGui::Text("- Transform component");
            }
            if (ecs_.has_component<LightComponent>(selected_entity)) {
                ImGui::Text("- Light component");
            }
            if (ecs_.has_component<RenderComponent>(selected_entity)) {
                ImGui::Text("- Render component");
            }
            if (ecs_.has_component<IdentifierComponent>(selected_entity)) {
                ImGui::Text("- Identifier component");
            }
            if (ecs_.has_component<HierarchyComponent>(selected_entity)) {
                ImGui::Text("- Hierarchy component");
            }
            if (ecs_.has_component<PhysicsComponent>(selected_entity)) {
                ImGui::Text("- Physics component");
            }
            if (ecs_.has_component<ScriptingComponent>(selected_entity)) {
                ImGui::Text("- Scripting component");
            }
            if (ecs_.has_component<CameraComponent>(selected_entity)) {
                ImGui::Text("- Camera component");
            }
            if (ecs_.has_component<SkyboxComponent>(selected_entity)) {
                ImGui::Text("- Skybox component");
            }
            ImGui::Dummy(ImVec2(0.0f, 3.0f));

            const char* addComponents[] = { "Physics Component", "Light Component", "Scripting Component" };
            static int addCurrentItem = -1;

            if (ImGui::BeginCombo("Add Component", addCurrentItem >= 0 ? addComponents[addCurrentItem] : "Select Component to Add")) {
                for (int i = 0; i < IM_ARRAYSIZE(addComponents); i++) {
                    bool hasComponent = false;
                    switch (i) {
                    case 0: hasComponent = ecs_.has_component<PhysicsComponent>(selected_entity); break;
                    case 1: hasComponent = ecs_.has_component<LightComponent>(selected_entity); break;
                    case 2: hasComponent = ecs_.has_component<ScriptingComponent>(selected_entity); break;
                    }

                    if (!hasComponent) {
                        if (ImGui::Selectable(addComponents[i], addCurrentItem == i)) {
                            switch (i) {
                            case 0:
                                ecs_.add_component<PhysicsComponent>(selected_entity);
                                break;
                            case 1:
                                ecs_.add_component<LightComponent>(selected_entity);
                                {
                                    auto& light_comp = ecs_.get_component<LightComponent>(selected_entity);
                                    light_comp->light.type_ = Light::LightType::LT_Point;
                                    if (ecs_.has_component<TransformComponent>(selected_entity)) {
                                        light_comp->light.pos_ = ecs_.get_component<TransformComponent>(selected_entity)->position;
                                    } else{
                                        light_comp->light.pos_ = { 0.0f, 0.0f, 0.0f };
                                    }
                                    light_comp->light.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
                                    light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
                                    light_comp->light.linear_att = 0.7f;
                                    light_comp->light.quad_att_ = 1.8f;
                                    light_comp->light.constant_att_ = 1.0f;
                                    light_comp->light.shininess_ = 32.0f;
                                    light_comp->light.strength_ = 1.0f;

                                    rs_.initLights();
                                }
                                break;
                            case 2:
                                ecs_.add_component<ScriptingComponent>(selected_entity);
                                break;
                            }
                            addCurrentItem = -1;
                        }
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Dummy(ImVec2(0.0f, 3.0f));

            // ---------------------- DELETE COMPONENT ----------------------

            const char* deleteComponents[] = { "Physics Component", "Light Component", "Scripting Component" };
            static int deleteCurrentItem = -1;

            if (ImGui::BeginCombo("Delete Component", deleteCurrentItem >= 0 ? deleteComponents[deleteCurrentItem] : "Select Component to Delete")) {
                for (int i = 0; i < IM_ARRAYSIZE(deleteComponents); i++) {
                    bool hasComponent = false;
                    switch (i) {
                    case 0: hasComponent = ecs_.has_component<PhysicsComponent>(selected_entity); break;
                    case 1: hasComponent = ecs_.has_component<LightComponent>(selected_entity); break;
                    case 2: hasComponent = ecs_.has_component<ScriptingComponent>(selected_entity); break;
                    }

                    if (hasComponent) {
                        if (ImGui::Selectable(deleteComponents[i], deleteCurrentItem == i)) {
                            switch (i) {
                            case 0:
                                ecs_.delete_component<PhysicsComponent>(selected_entity);
                                break;
                            case 1:
                                ecs_.delete_component<LightComponent>(selected_entity);
                                break;
                            case 2:
                                ecs_.delete_component<ScriptingComponent>(selected_entity);
                                break;
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

        if (ImGui::Button("Delete Entity") || input_->isKeyPressed(Input::SUPR)) {
            if (ecs_.has_component<HierarchyComponent>(selected_entity)) {
                auto& hierarchy = ecs_.get_component<HierarchyComponent>(selected_entity);

                for (int parentId : hierarchy->getParents()) {
                    if (ecs_.has_component<HierarchyComponent>(parentId)) {
                        ecs_.get_component<HierarchyComponent>(parentId)->removeChild(selected_entity);
                    }
                }

                for (int childId : hierarchy->getChildren()) {
                    if (ecs_.has_component<HierarchyComponent>(childId)) {
                        ecs_.get_component<HierarchyComponent>(childId)->removeParent(selected_entity);
                    }
                }

                hierarchy->clearParents();
                hierarchy->clearChildren();
            }

            ecs_.destroy_entity(selected_entity);
            selected_entity = 0;
            ResetIds();
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

            if (path.extension() == ".json") {
                engine_.save_manager.setCurrentScene(path.string());
                engine_.save_manager.loadFromFile(engine_.save_manager.getCurrentScene());

                AddConsoleMessage(path.string() + " loaded");
            }

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

    static int curr_type_draw = 0; 

    if (ImGui::BeginMenuBar()) {

        ImGui::Image(m_SettingsIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        if (ImGui::BeginMenu("Settings")) {
            
            ImGui::Checkbox("FPS", &show_fps);

            if (ImGui::MenuItem("Toggle Full Screen [F11]")) {
                window_->toggleFullScreen(*input_, Input::None, true);
            }

            if (ImGui::BeginMenu("Console")) {
                ImGui::Checkbox("Console Window", &console_window);
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save Scene [CTRL + S]")) {

                if (engine_.save_manager.getCurrentScene().empty()) {
                    engine_.save_manager.saveToFile("../stages/saves/scene_0.json");
                }
                else {
                    engine_.save_manager.saveToFile(engine_.save_manager.getCurrentScene());
                }

                AddConsoleMessage("Saving current scene");
            }

            if (ImGui::MenuItem("New Scene  [CTRL + N]")) {
                engine_.save_manager.newFile();
                AddConsoleMessage("New scene created");
            }

            ImGui::EndMenu();
        }

        ImGui::Image(m_EditIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        if (ImGui::BeginMenu("Edit")) {

            if (ImGui::BeginMenu("Render Type")) {
                if (ImGui::MenuItem("Forward Render", nullptr)) {
                    rs_.use_deferred_shading = false;
                }
                if (ImGui::MenuItem("Deferred Render", nullptr)) {
                    rs_.use_deferred_shading = true;
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

            if (ImGui::BeginMenu("Post Proccess")) {
                ImGui::Checkbox("Post Process Window", &post_process_window);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Fog")) {
                ImGui::Checkbox("Fog Window", &fog_window);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("LSystem")) {
                ImGui::Checkbox("LSystem Window", &l_system_window);
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::Image(m_ToolsIcon.getID(), { 22.0f, 22.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
        ImGui::SameLine();
        if (ImGui::BeginMenu("Tools")) {

            if (ImGui::MenuItem("Add Camera")) {
                rs_.addCamera();
                ResetIds();

                AddConsoleMessage("Camera added");
            }

            if (ImGui::MenuItem("Add Skybox")) {
                rs_.addSkybox();
                rs_.initSkybox();
                ResetIds();

                AddConsoleMessage("Skybox added");
            }

            if (ImGui::BeginMenu("Add Geometry")) {

                if (ImGui::MenuItem("Sphere")) {
                    rs_.createSphere();
                    rs_.initRenderTypes();
                    ResetIds();

                    AddConsoleMessage("Sphere added to scene");
                }

                if (ImGui::MenuItem("Cube 8v")) {
                    rs_.createCube8v();
                    rs_.initRenderTypes();
                    ResetIds();

                    AddConsoleMessage("Cube 8v added to scene");
                }

                if (ImGui::MenuItem("Cube 24v")) {
                    rs_.createCube24v();
                    rs_.initRenderTypes();
                    ResetIds();

                    AddConsoleMessage("Cube 24v added to scene");
                }

                if (ImGui::MenuItem("Quad")) {
                    rs_.createQuad();
                    rs_.initRenderTypes();
                    ResetIds();

                    AddConsoleMessage("Quad added to scene");
                }

                if (ImGui::MenuItem("Terrain")) {
                    rs_.createTerrain();
                    rs_.initRenderTypes();
                    ResetIds();

                    AddConsoleMessage("Terrain added to scene");
                }

                if (ImGui::MenuItem("Water")) {
                    rs_.createWater();
                    rs_.initRenderTypes();
                    ResetIds();

                    AddConsoleMessage("Water added to scene");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Add Light")) {

                if (ImGui::MenuItem("Point Light")) {
                    rs_.createPointLight();
                    rs_.initLights();
                    ResetIds();

                    AddConsoleMessage("Point Light added to scene");
                }

                if (ImGui::MenuItem("Spot Light")) {
                    rs_.createSpotLight();
                    rs_.initLights();
                    ResetIds();

                    AddConsoleMessage("Spot Light added to scene");
                }

                if (ImGui::MenuItem("Directional Light")) {
                    rs_.createDirectionalLight();
                    rs_.initLights();
                    ResetIds();

                    AddConsoleMessage("Directional Light added to scene");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Add Surface")) {

                ImGui::Checkbox(("Add surface Window"), &add_surface_window);

                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Add Mesh")) {

                ImGui::Checkbox(("Add mesh Window"), &add_mesh_window);
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Spawn Entities in terrain")) {

                ImGui::Checkbox(("Spawn Entities Window"), &add_entities_terrain_window);

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (rs_.use_deferred_shading) {

            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y), ImVec2(p.x, p.y + ImGui::GetTextLineHeight()), IM_COL32(255, 255, 255, 255), 1.0f);
            ImGui::SameLine();

            ImGui::Spacing();
            ImGui::Spacing();

            if (ImGui::RadioButton("Albedo", rs_.deferred.deferred_option == 0)) {
                rs_.deferred.deferred_option = 0;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Normal", rs_.deferred.deferred_option == 1)) {
                rs_.deferred.deferred_option = 1;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Position", rs_.deferred.deferred_option == 2)) {
                rs_.deferred.deferred_option = 2;
            }
        }

        if (show_fps) {
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y), ImVec2(p.x, p.y + ImGui::GetTextLineHeight()), IM_COL32(255, 255, 255, 255), 1.0f);
            ImGui::SameLine();

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Text("FPS: %0.1f", 1000.0 / engine_.dt);
            //ImGui::Text("Delta time: %0.3f ms", global.dt);
        }

        float spacing = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("T33Engine - Sergio Madaleno Figueroa & Fede Sanjuan").x;
        ImGui::Dummy(ImVec2(spacing, 0));
        ImGui::SameLine();
        ImGui::Text("T33Engine - Sergio Madaleno Figueroa & Fede Sanjuan");

        ImGui::EndMenuBar();
    }

    // ---

    if (input_->isKeyDown(Input::CTRL) && input_->isKeyPressed(Input::S)) {
        if (engine_.save_manager.getCurrentScene().empty()) {
            engine_.save_manager.saveToFile("../stages/saves/scene_0.json");
        }
        else {
            engine_.save_manager.saveToFile(engine_.save_manager.getCurrentScene());
        }

        AddConsoleMessage("Saving current scene");
    }

    if (input_->isKeyDown(Input::CTRL) && input_->isKeyPressed(Input::N)) {
        engine_.save_manager.newFile();
        AddConsoleMessage("New scene created");
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
    if (ImGui::Button(!vertex_file_path.empty() ? vertex_file_name.c_str() : ".vs drop here", { 200.0f, 40.0f })) {}

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
    if (ImGui::Button(!fragment_file_path.empty() ? fragment_file_name.c_str() : ".fs drop here", { 200.0f, 40.0f })) {}

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
        engine_.post_process.render(new_pp_shader);

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

        if (use_grey) ImGui::DragFloat("Grey##1", &engine_.post_process.grey_, 0.01f, 0.0f, 1.0f, "%.3f");
        if (use_negative)  ImGui::DragFloat("Negative##1", &engine_.post_process.negative_, 0.01f, 0.5f, 1.0f, "%.3f");
        if (use_waves)  ImGui::DragFloat("Waves##1", &engine_.post_process.waves_, 0.001f, 0.00f, 0.09f, "%.3f");
        if (use_blur) ImGui::DragFloat("Blur##1", &engine_.post_process.blur_, 0.00001f, 0.00f, 0.035f, "%.3f");
        if (use_kernel) ImGui::DragInt("Kernel types##1", &engine_.post_process.kernel_types_, 0.1f, 0, 2, "%d");

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
    ImGui::Checkbox("Activate", &rs_.use_fog);
    ImGui::Dummy({ 0.0f, 5.0f });

    ImGui::DragFloat("Density", &rs_.fog_density, 0.001f, 0.0f, 2.0f, "%.3f");
    ImGui::DragFloat("Gradient", &rs_.fog_gradient, 0.001f, 0.0f, 10.0f, "%.3f");

    ImGui::Dummy({ 0.0f, 5.0f });
    
    float color[3] = {
                    rs_.sky_colour.r / 255.0f,
                    rs_.sky_colour.g / 255.0f,
                    rs_.sky_colour.b / 255.0f
    };

    if (ImGui::ColorPicker3("Sky Colour", color)) {
        rs_.sky_colour.x = color[0];
        rs_.sky_colour.y = color[1];
        rs_.sky_colour.z = color[2];
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

void UIManager::ResetIds()
{
    int new_id = 0;
    size_t max_components = std::max({
        ecs_.getComponentList<IdentifierComponent>().size(),
        ecs_.getComponentList<HierarchyComponent>().size(),
        ecs_.getComponentList<RenderComponent>().size(),
        ecs_.getComponentList<TransformComponent>().size(),
        ecs_.getComponentList<LightComponent>().size(),
        ecs_.getComponentList<PhysicsComponent>().size(),
        ecs_.getComponentList<CameraComponent>().size()
        });

    for (size_t i = 0; i < max_components; ++i) {
        if (ecs_.has_component<IdentifierComponent>(i)) {
            ecs_.get_component<IdentifierComponent>(i)->id = new_id++;
        }
    }
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
    if (ImGui::Button(!obj_file_path.empty() ? obj_file_name.c_str() : ".obj drop here", { 200.0f, 40.0f })) {}

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
            rs_.createMesh(obj_file_path, texture_folder_path);
            rs_.initRenderTypes();

            ResetIds();
            AddConsoleMessage(obj_file_name + " added to scene");

            obj_file_path.clear();
            obj_file_name.clear();
            texture_folder_path.clear();
            texture_folder_name.clear();
        }
    }

    ImGui::End();
}

void UIManager::AddSurfaceWindow()
{
    ImGui::Begin("Add Surface");

    static std::vector<float> points;
    static int num_points = 1;

    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::SliderInt("Number of Points", &num_points, 1, 30);

    if (points.size() != num_points * 3) {
        points.resize(num_points * 3, 0.0f);
    }

    for (int i = 0; i < points.size(); i += 3) {
        char label[32];
        snprintf(label, 32, "Point %d", (i / 3) + 1);

        ImGui::InputFloat3(label, &points[i]);
    }
    ImGui::Dummy({ 0.0f, 2.0f });

    static int num_heights = 4;
    static int num_revs = 4;
    ImGui::InputInt("Num heights", &num_heights);
    ImGui::InputInt("Num Revs", &num_revs);

    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::Separator();
    ImGui::Dummy({ 0.0f, 2.0f });

    if (ImGui::Button("Add surface")) {
        TransformComponent tc;

        rs_.createSurface(points, tc, num_heights, num_revs);
        rs_.initRenderTypes();
        ResetIds();

        AddConsoleMessage("Surface added to scene");
    }

    ImGui::End();
}

void UIManager::AddEntitiesTerrainWindow()
{
    ImGui::Begin("Spawn Entities");

    static int entity_id = 0;
    static float spawn_prob = 0.008f;
    static float spawn_height = -4.0f;

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::InputInt("Terrain ID", &entity_id);
    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::InputFloat("Spawn probaility", &spawn_prob);
    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::InputFloat("Spawn height", &spawn_height);
    ImGui::Dummy({ 0.0f, 5.0f });

    const char* render_types[] = { "Sphere", "Cube8v", "Cube24v", "Quad", "Mesh"};
    static int selected_type = 0;

    if (ImGui::BeginCombo("Entity type", render_types[selected_type])) {
        for (int n = 0; n < IM_ARRAYSIZE(render_types); ++n) {
            bool is_selected = (selected_type == n);
            if (ImGui::Selectable(render_types[n], is_selected)) {
                selected_type = n; 
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    RenderType entity_type = RenderType::RT_Sphere;
    switch (selected_type) {
    case 0: entity_type = RenderType::RT_Sphere; break;
    case 1: entity_type = RenderType::RT_Cube8v; break;
    case 2: entity_type = RenderType::RT_Cube24v; break;
    case 3: entity_type = RenderType::RT_Quad; break;
    case 4: entity_type = RenderType::RT_Mesh; break;
    }

    static std::string obj_file_path;
    static std::string obj_file_name;
    static std::string texture_folder_path;
    static std::string texture_folder_name;

    if (entity_type == RenderType::RT_Mesh) {
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Drag and Drop obj file:");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        if (ImGui::Button(!obj_file_path.empty() ? obj_file_name.c_str() : ".obj drop here", { 200.0f, 40.0f })) {}

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
    }

    ImGui::Dummy({ 0.0f, 2.0f });
    ImGui::Separator();
    ImGui::Dummy({ 0.0f, 2.0f });

    if (entity_type == RenderType::RT_Mesh) {
        if (ImGui::Button("Spawn")) {
            rs_.spawnMeshInTerrain(entity_id, obj_file_path, texture_folder_path, spawn_prob, spawn_height);

            ResetIds();
            AddConsoleMessage(obj_file_name + " added to scene");

            obj_file_path.clear();
            obj_file_name.clear();
            texture_folder_path.clear();
            texture_folder_name.clear();
        }
    }
    else {
        if (ImGui::Button("Spawn")) {
            rs_.spawnEntitiesInTerrain(entity_id, entity_type, spawn_prob, spawn_height);

            ResetIds();
            AddConsoleMessage("Entities added to scene");
        }
    }

    ImGui::End();
}

//--

void UIManager::Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity) {

    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    ImGuizmo::BeginFrame();
    if (ImGui::Begin("GameWindow", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        auto ti = ecs_.getComponentList<TransformComponent>().begin();
        auto li = ecs_.getComponentList<LightComponent>().begin();
        auto tiend = ecs_.getComponentList<TransformComponent>().end();
        auto liend = ecs_.getComponentList<LightComponent>().end();

        int entity_id = 0;

        for (; ti != tiend; ++ti, ++entity_id) {
            
            if (entity_id == selected_entity && ti->has_value()) {

                auto& transform = ecs_.get_component<TransformComponent>(selected_entity);

                if (ecs_.has_component<LightComponent>(selected_entity)) {
                    auto& light_comp = ecs_.get_component<LightComponent>(selected_entity);
                    light_comp->light.pos_ = transform->position;
                    light_comp->light.dir_ = glm::mat3(transform->rotation) * glm::vec3(0.0f, 0.0f, 1.0f);
                }

                if (ecs_.has_component<CameraComponent>(selected_entity)) {
                    auto& camera_comp = ecs_.get_component<CameraComponent>(selected_entity);

                    transform->position = camera_comp->camera.getPosition();

                    camera_comp->camera.setPosition(transform->position);
                }

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform->position);
                model = model * glm::mat4_cast(transform->rotation);
                model = glm::scale(model, transform->scale);

                float objectMatrix[16];
                std::memcpy(objectMatrix, glm::value_ptr(model), sizeof(objectMatrix));

                //if (!ecs_.has_component<CameraComponent>(selected_entity)) {
                    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                        mCurrentGizmoOperation, ImGuizmo::LOCAL,
                        objectMatrix, nullptr);
                //}

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

            if (!input_->isMouseButtonDown(Input::MOUSE_RIGHT)) {
                if (input_->isKeyPressed(Input::Q) && selected_entity) {
                    mCurrentGizmoOperation = ImGuizmo::SCALE;
                }

                if (input_->isKeyPressed(Input::W) && selected_entity) {
                    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                }

                if (input_->isKeyPressed(Input::E) && selected_entity) {
                    mCurrentGizmoOperation = ImGuizmo::ROTATE;
                }
            }
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
        LSystem lsystem(ecs_, rs_);
        lsystem.angle_ = angle;
        lsystem.cube_size_ = cube_size;
        lsystem.spacing_ = spacing;
        lsystem.starting_pos_ = starting_pos;
        lsystem.starting_rot_ = glm::normalize(starting_rot);

        lsystem.init(axiom, rule_map, iterations);
    }

    ImGui::Dummy({ 0.0f, 5.0f });

    if (ImGui::Button("Delete all entities")) {
        ecs_.remove_entities();
    }

    ImGui::Dummy({ 0.0f, 5.0f });
    ImGui::End();
}