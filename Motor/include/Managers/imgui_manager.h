#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

#include "Core/texture.h"
#include "Core/shader.h"
#include "utils.h"
#include <../deps/glm/glm.hpp>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window);
    void Render(GLFWwindow* window);
    void Cleanup();

    void SetImguiTheme();

    // ---

    void ImguiWindow(GLFWwindow* window);
    void GameWindow();
    void HierarchyWindow();
    void InspectorWindow();
    void ContentBrowserWindow();
    void TopBarWindow();

    void PostProcessWindow();
    void FogWindow();
    void ShadowMapWindow();
    void SkyboxWindow();
    void ConsoleWindow();
    void LoadSceneWindow();
    void AddMeshWindow();

    void AddConsoleMessage(const std::string& message);
    void Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity);

private:

    int selected_entity;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    std::vector<std::string> consoleMessages;

    Texture m_DirectoryIcon, m_FileIcon, m_EntityIcon, m_BackIcon,
    m_ToolsIcon, m_LightIcon, m_CameraIcon, m_TransformIcon, m_SettingsIcon,
    m_TagIcon, m_RenderIcon, m_EditIcon, m_PuzzleIcon, m_DeleteIcon;

    float padding;
    float thumbnailSize;
    
    //--

    bool post_process_window;
    bool skybox_window;
    bool console_window;
    bool fog_window;
    bool shadow_map_window;

    Shader new_pp_shader;
    Shader grey_sh, negative_sh, waves_sh, blur_sh, kernel_sh;
    bool post_process_compiled;

    bool load_scene;
    bool add_mesh_window;
};

#endif // _UI_MANAGER_H_
