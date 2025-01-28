
#include "Managers/imgui_manager.h"
#include "utils.h"
#include "Render/post_processing.h"
#include "Components/render_component.h"
#include "Components/light_component.h"
#include "Components/transform_component.h"
#include "Managers/save_manager.h"

#include "Core/window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
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

ImGuiManager::ImGuiManager() 
    :
    m_BaseDirectory("../"),
    m_CurrentDirectory(m_BaseDirectory)
{
}

ImGuiManager::~ImGuiManager() {
    Cleanup();
}

void ImGuiManager::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::SetImguiTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Background
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

    // Frame background (e.g., input boxes, sliders)
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); 
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); 

    // Title bar
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.45f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); 
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); 
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f); 
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); 

    // Checkboxes, radio buttons, sliders
    colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); 
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

    // Separator
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.75f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);

    // Hover effects for hovered items
    style.FrameRounding = 4.0f; 
    style.WindowRounding = 5.0f; 
    style.ChildRounding = 4.0f; 

    // Docking colors
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f); 
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);

    // Adjust separators for docking clarity
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f); 
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);

}


void ImGuiManager::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    SetImguiTheme();

    // Initialize content browser
    fileDialog.SetTitle("File Browser");
    fileDialog.SetTypeFilters({
        ".h", ".cc", ".hpp", ".cpp",
        ".png", ".jpg", ".mtl",
        ".lua", ".json", ".obj",
        ".vs", ".fs",
        });

    m_DirectoryIcon.loadTexture("../assets/textures/imgui/DirectoryIcon.png");
    m_FileIcon.loadTexture("../assets/textures/imgui/FileIcon.png");
    m_EntityIcon.loadTexture("../assets/textures/imgui/cube.png");
    m_EntityLightBulbIcon.loadTexture("../assets/textures/imgui/lb.png");

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

void ImGuiManager::Render(GLFWwindow* window, DataImgui& di) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Render ImGui windows
    ImguiWindow(window);
    GameWindow(di);
    HierarchyWindow(di);
    InspectorWindow(di);
    ContentBrowserWindow(di);
    TopBarWindow(di);

    // Render draw data
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Handle viewports
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiManager::ImguiWindow(GLFWwindow* window) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainDockspace_", nullptr, dockspace_flags);

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace_");
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

        ImGui::DockBuilderDockWindow("Menu", dock_top);
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

    GLuint captureTexture = di.game_texture;

    if (captureTexture) {
        ImVec2 uv0(0.0f, 1.0f);
        ImVec2 uv1(1.0f, 0.0f);

        ImGui::Image((ImTextureID)(intptr_t)captureTexture, content_avail, uv0, uv1);
    }

    ImGui::End();
}

void ImGuiManager::HierarchyWindow(DataImgui& di) {
    ImGui::Begin("Hierarchy", nullptr);

    auto& loc_ecs = di.ecs;

    //--
    auto ri = loc_ecs->getComponentList<RenderComponent>().begin();
    auto ti = loc_ecs->getComponentList<TransformComponent>().begin();
    auto riend = loc_ecs->getComponentList<RenderComponent>().end();
    auto tiend = loc_ecs->getComponentList<TransformComponent>().end();
    auto li = loc_ecs->getComponentList<LightComponent>().begin();
    auto liend = loc_ecs->getComponentList<LightComponent>().end();

    int entity_id = 0;

    int entity_mesh = 0;
    int entity_terrain = 0;
    int entity_sphere = 0;
    int entity_quad = 0;
    int entity_cube = 0;
    int entity_spotlight = 0;
    int entity_pointlight = 0;
    int entity_dirlight = 0;

    for (; ri != riend; ++ri, entity_id++) {

        if (ri->has_value() && ri != riend) {
            auto& render_comp = *ri;

            if ((render_comp->type_entity_ != EntityType::ET_PointLight) && (render_comp->type_entity_ != EntityType::ET_SpotLight)) {
                ImGui::Image(m_EntityIcon.getID(), { 25.0f, 25.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            }else {
                ImGui::Image(m_EntityLightBulbIcon.getID(), { 23.0f, 23.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            }
            ImGui::SameLine();

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:

                if (render_comp->type_entity_ != EntityType::ET_SpotLight && render_comp->type_entity_ != EntityType::ET_PointLight) {

                    if (ImGui::Selectable(("Mesh " + std::to_string(entity_mesh++)).c_str(), selected_entity == entity_id))
                        selected_entity = entity_id;
                    break;
                }

                if (render_comp->type_entity_ == EntityType::ET_PointLight) {
                    if (ImGui::Selectable(("Point Light " + std::to_string(entity_pointlight++)).c_str(), selected_entity == entity_id))
                        selected_entity = entity_id;
                    break;
                }

                if (render_comp->type_entity_ == EntityType::ET_SpotLight) {
                    if (ImGui::Selectable(("Spot Light " + std::to_string(entity_spotlight++)).c_str(), selected_entity == entity_id))
                        selected_entity = entity_id;
                    break;
                }

                break;

            case RenderType::RT_Terrain:
                if (ImGui::Selectable(("Terrain " + std::to_string(entity_terrain++)).c_str(), selected_entity == entity_id))
                    selected_entity = entity_id;

                break;

            case RenderType::RT_Sphere:
                if (ImGui::Selectable(("Sphere " + std::to_string(entity_sphere++)).c_str(), selected_entity == entity_id))
                    selected_entity = entity_id;

                break;

            case RenderType::RT_Quad:
                if (ImGui::Selectable(("Quad " + std::to_string(entity_quad++)).c_str(), selected_entity == entity_id))
                    selected_entity = entity_id;

                break;

            case RenderType::RT_Cube:
                if (ImGui::Selectable(("Cube " + std::to_string(entity_cube++)).c_str(), selected_entity == entity_id))
                    selected_entity = entity_id;

                break;
            }
        }
    }
    
    for (; li != liend; ++li, entity_id++) {

        if (li->has_value() && li != liend) {
            auto& light_comp = *li;

            if (light_comp->light_info.type_ == LightType::LT_Directional) {
                ImGui::Image(m_EntityLightBulbIcon.getID(), { 23.0f, 23.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
                ImGui::SameLine();

                if (ImGui::Selectable(("Directional Light " + std::to_string(entity_dirlight++)).c_str(), selected_entity == entity_id))
                    selected_entity = entity_id;

            }
         }
        
    }

    ImGui::End();
}

void ImGuiManager::InspectorWindow(DataImgui& di) {
    ImGui::Begin("Inspector");

    if (di.ecs->has_component<RenderComponent>(selected_entity)) {

        Gizmo(di.cam.getViewMatrix(), di.cam.getProjectionMatrix(), selected_entity, di);

        ImGui::Dummy(ImVec2(10.0f, 15.0f));

        if (di.ecs->has_component<TransformComponent>(selected_entity)) {
            auto& transform = di.ecs->get_component<TransformComponent>(selected_entity);

            if (transform.has_value()) {
                ImGui::DragFloat("Position X", &transform->position.x, 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Position Y", &transform->position.y, 0.1f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Position Z", &transform->position.z, 0.1f, -1000.0f, 1000.0f);
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 15.0f));

        if (di.ecs->has_component<LightComponent>(selected_entity)) {
            auto& light_component = di.ecs->get_component<LightComponent>(selected_entity);

            if (light_component.has_value()) {

                ImGui::Separator();

                glm::vec3& light_color = light_component->light_info.light_color_;

                float color[3] = {
                    light_color.r / 255.0f,
                    light_color.g / 255.0f,
                    light_color.b / 255.0f
                };

                ImGui::Dummy(ImVec2(0.0f, 5.0f));

                if (ImGui::ColorPicker3("Light Color", color)) {
                    light_color.r = color[0] * 255.0f;
                    light_color.g = color[1] * 255.0f;
                    light_color.b = color[2] * 255.0f;
                }

                ImGui::Dummy(ImVec2(0.0f, 5.0f));

                ImGui::Text("Current Light Color: R: %.2f, G: %.2f, B: %.2f",
                    light_color.r, light_color.g, light_color.b);

                ImGui::Dummy(ImVec2(0.0f, 5.0f));
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        // Change shader
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Drag and Drop Vertex and Fragment:");
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        static std::string vertex_file_name;
        static char* vertex_content = nullptr;
        if (ImGui::Button(!vertex_file_name.empty() ? std::string("" + vertex_file_name).c_str() : "VS Drop here", { 120.0f, 40.0f })) {}

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::filesystem::path filePath = (const wchar_t*)payload->Data;
                if (filePath.extension() == ".vs") {
                    vertex_file_name = filePath.filename().string();
                    vertex_content = ReadFile(filePath.string().c_str());
                }
            }
            ImGui::EndDragDropTarget();
        }

        static std::string fragment_file_name;
        static char* fragment_content = nullptr;
        ImGui::SameLine();
        if (ImGui::Button(!fragment_file_name.empty() ? std::string("" + fragment_file_name).c_str() : "FS Drop here", { 120.0f, 40.0f })) {}

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::filesystem::path filePath = (const wchar_t*)payload->Data;
                if (filePath.extension() == ".fs") {
                    fragment_file_name = filePath.filename().string();
                    fragment_content = ReadFile(filePath.string().c_str());
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (vertex_content && fragment_content) {

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            if (ImGui::Button("Compile")) {
                Shader newShader;
                newShader.compile(vertex_content, fragment_content);
                di.ecs->get_component<RenderComponent>(selected_entity)->shader = newShader;

                vertex_file_name.clear();
                fragment_file_name.clear();

                vertex_content = nullptr;
                fragment_content = nullptr;
          
            }
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }

        //----

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        if (selected_entity && (ImGui::Button("Delete Entity") || selected_entity && di.input->isKeyPressed(Input::SUPR))) {
            if (di.ecs->get_component<RenderComponent>(selected_entity)->type_entity_ != EntityType::ET_PointLight
                && di.ecs->get_component<RenderComponent>(selected_entity)->type_entity_ != EntityType::ET_SpotLight) {
                di.ecs->destroy_entity(selected_entity);
            }
            else {
                di.ecs->destroy_entity(selected_entity);
                di.render_system->number_lights--;
            }
        }
    }

    ImGui::End();
}

void ImGuiManager::ContentBrowserWindow(DataImgui& di) {
    ImGui::Begin("ContentBrowser");

    if (std::filesystem::canonical(m_CurrentDirectory) != std::filesystem::canonical(m_BaseDirectory))
    {
        if (ImGui::Button("<-"))
        {
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnailSize = 128.0f;
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

void ImGuiManager::TopBarWindow(DataImgui& di) {

    ImGui::Begin("Menu", nullptr,
        ImGuiWindowFlags_MenuBar);

    auto& loc_ecs = *(di.ecs);
    static int curr_type_draw = 0; 

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Settings")) {
            
            if (ImGui::MenuItem("Toggle Full Screen [F11]")) {
                di.window->toggleFullScreen(*di.input, Input::None, true);
            }

            if (ImGui::MenuItem("Save Scene [CTRL + S]")) {
                di.save_manager.saveToFile(*di.ecs, "../saves/scene_1.json");
            }

            if (ImGui::MenuItem("Load Scene")) {
                di.save_manager.loadFromFile(*di.ecs, "../saves/scene_1.json",*di.render_system,*di.shader);
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

            if (ImGui::BeginMenu("Add Geometry")) {

                if (ImGui::MenuItem("Sphere")) {
                    di.render_system->createSphere(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                if (ImGui::MenuItem("Cube 8v")) {
                    di.render_system->createCube8v(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                if (ImGui::MenuItem("Cube 24v")) {
                    di.render_system->createCube24v(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                if (ImGui::MenuItem("Quad")) {
                    di.render_system->createQuad(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                if (ImGui::MenuItem("Terrain")) {
                    di.render_system->createTerrain(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Add Light")) {

                if (ImGui::MenuItem("Point Light")) {
                    di.render_system->createPointLight(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                if (ImGui::MenuItem("Spot Light")) {
                    di.render_system->createSpotLight(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                if (ImGui::MenuItem("Directional Light")) {
                   
                    di.render_system->createDirectionalLight(loc_ecs, *di.shader);
                    di.render_system->initEntities(loc_ecs);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void ImGuiManager::Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity, DataImgui& di) {

    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    auto& loc_ecs = di.ecs;

    ImGuizmo::BeginFrame();
    if (ImGui::Begin("GameWindow", nullptr, ImGuiWindowFlags_NoTitleBar)) {
        ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        auto ti = loc_ecs->getComponentList<TransformComponent>().begin();
        auto li = loc_ecs->getComponentList<LightComponent>().begin();
        auto tiend = loc_ecs->getComponentList<TransformComponent>().end();
        auto liend = loc_ecs->getComponentList<LightComponent>().end();

        int entity_id = 0;

        for (; ti != tiend; ++ti, ++entity_id) {
            
            if (entity_id == selected_entity && ti->has_value()) {

                auto& transform = loc_ecs->get_component<TransformComponent>(selected_entity);

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
