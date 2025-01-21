#include "imgui_manager.h"
#include "utils.h"
#include "Render/post_processing.h"
#include "window.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static RenderData rd;
static PostProcessing postProcc;

ImGuiManager::ImGuiManager() {}

ImGuiManager::~ImGuiManager() {
    Cleanup();
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

void ImGuiManager::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::Render(GLFWwindow* window, PostProcessing& postProcc) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImguiWindow(window);
    GameWindow(postProcc);
    Hierarchy();  
    Inspector(); 
    ContentBrowser();  
    TopBar();

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
        ImGuiID dock_top = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Up, 0.1f, nullptr, &dock_main);
        ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, nullptr, &dock_main);
        ImGuiID dock_right = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.3f, nullptr, &dock_main);
        ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.2f, nullptr, &dock_main);


        ImGui::DockBuilderDockWindow("TopBar", dock_top);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
        ImGui::DockBuilderDockWindow("ContentBrowser", dock_bottom);
        ImGui::DockBuilderDockWindow("GameWindow", dock_main);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
}

void ImGuiManager::GameWindow(PostProcessing& postProcc) {
    ImGui::Begin("GameWindow", nullptr);
    ImVec2 content_avail = ImGui::GetContentRegionAvail();

    GLuint gameTex = postProcc.getGamefBO();
    if (gameTex) {
        ImVec2 uv0(0.0f, 1.0f);
        ImVec2 uv1(1.0f, 0.0f);

        ImGui::Image((ImTextureID)(intptr_t)gameTex, content_avail, uv0, uv1);
    }
    else {
        ImGui::Text("Invalid game texture.");
    }

    ImGui::End();
}

void ImGuiManager::Hierarchy() {
    ImGui::Begin("Hierarchy");


    ImGui::End();
}

void ImGuiManager::Inspector() {
    ImGui::Begin("Inspector");

    
    ImGui::End();
}

void ImGuiManager::ContentBrowser() {
    ImGui::Begin("ContentBrowser");

    ImGui::End();
}

void ImGuiManager::TopBar() {
    ImGui::Begin("TopBar");


    ImGui::End();
}

