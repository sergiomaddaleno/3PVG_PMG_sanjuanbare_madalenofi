#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

#include "Core/texture.h"
#include "Core/shader.h"
#include "utils.h"
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum Effect;

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
    void SkyboxWindow();
    void ConsoleWindow();
    void LoadSceneWindow();
    void AddMeshWindow();

    void Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity);

private:

    int selected_entity;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    Texture m_DirectoryIcon;
    Texture m_FileIcon;
    Texture m_EntityIcon;
    Texture m_BackIcon;

    float padding;
    float thumbnailSize;
    
    //--

    bool post_process_window;
    bool skybox_window;
    bool console_window;
    bool fog_window;

    Shader new_pp_shader;
    Shader grey_sh, negative_sh, waves_sh, blur_sh, kernel_sh;
    bool post_process_compiled;
    Effect current_effect;

    bool load_scene;
    bool add_mesh_window;
};

#endif // _UI_MANAGER_H_
