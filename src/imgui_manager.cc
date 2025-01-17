
#include "imgui_manager.h"
#include "utils.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

void ImGuiManager::Render(GLFWwindow* window, RenderData& rd, PostProcessing& postProcc) {

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    //SetupDocking();

    GameWindow(rd, postProcc);
    //Hierarchy();
    //Inspector();
    //ContentBrowser();
    //TopBar();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiManager::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::SetupDocking() {

    static ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoWindowMenuButton;

    ImGuiID dockspaceID = 0;
    ImGuiID leftID = 0;
    ImGuiID centerID = 0;
    ImGuiID rightID = 0;

    dockspaceID = ImGui::GetID("Dock");

    if (!ImGui::DockBuilderGetNode(dockspaceID)) {
        // Remove existing node and create a new one
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

        // Create the main docking space (top-level node)
        ImGuiID mainID = dockspaceID;

        // Split the main space into left and right nodes
        leftID = ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Left, 0.25f, nullptr, &centerID); // Left 25%
        rightID = ImGui::DockBuilderSplitNode(centerID, ImGuiDir_Right, 0.5f, nullptr, nullptr); // Right 50%

        // Dock windows to each area
        //ImGui::DockBuilderDockWindow("TopBar", ImGuiDir_Up);          // Dock "TopBar" at the top (above all windows)
        //ImGui::DockBuilderDockWindow("Hierarchy", leftID);            // Dock "Hierarchy" to the left part
        //ImGui::DockBuilderDockWindow("Inspector", rightID);           // Dock "Inspector" to the right part
        ImGui::DockBuilderDockWindow("GameWindow", centerID);         // Dock "GameWindow" to the center part
        //ImGui::DockBuilderDockWindow("ContentBrowser", ImGuiDir_Down); // Dock "ContentBrowser" at the bottom

        // Finish the docking setup
        ImGui::DockBuilderFinish(mainID);
    }

    // Render the docking space
    ImGui::DockSpace(dockspaceID, ImVec2(0, 0), dockFlags);
}


void ImGuiManager::GameWindow(RenderData &rd, PostProcessing& postProcc) {
  
    ImGui::Begin("GameWindow", nullptr);
    ImVec2 content_avail = ImGui::GetContentRegionAvail();

    GLuint gameTex = postProcc.getGamefBO();
    ImVec2 uv0(0.0f, 1.0f);
    ImVec2 uv1(1.0f, 0.0f);

    ImGui::Image((ImTextureID)(intptr_t)gameTex, content_avail, uv0, uv1);

    ImGui::End();
}


void ImGuiManager::Hierarchy()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );


    ImGui::End();
}

void ImGuiManager::Inspector()
{
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);


    ImGui::End();
}

void ImGuiManager::ContentBrowser()
{
    ImGui::Begin("ContentBrowser", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);


    ImGui::End();
}

void ImGuiManager::TopBar()
{
    ImGui::Begin("TopBar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);


    ImGui::End();
}

