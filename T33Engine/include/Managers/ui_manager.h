/**
 * @file ui_manager.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __UI_MANAGER__
#define __UI_MANAGER__

#include "Render/render_system.h"
#include "../Systems/ecs_manager.h"
#include "Core/texture.h"
#include "Core/shader.h"
#include "Core/input.h"
#include "utils.h"
#include <../deps/glm/glm.hpp>
#include <filesystem>
#include <optional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class UIManager
  * @brief Manages the user interface elements.
  */
class UIManager {
public:
    UIManager(ECSManager& ecs, RenderSystem& rs);
    ~UIManager();

    void Init(Window* window, Input* input);
    void Render();
    void Cleanup();

    void SetImguiTheme();

    // ---

    void ImguiWindow();
    void GameWindow();
    void HierarchyWindow();
    void InspectorWindow();
    void ContentBrowserWindow();
    void TopBarWindow();

    void PostProcessWindow();
    void FogWindow();
    void SkyboxWindow();
    void ConsoleWindow();
    void AddMeshWindow();
    void AddSurfaceWindow();
    void AddEntitiesTerrainWindow();

    void LSystemWindow();

    void HierarchyTree(const std::string& name, int entity_id);
    void AddConsoleMessage(const std::string& message);
    void ResetIds();
    void Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity);

private:
    ECSManager& ecs_;
    RenderSystem& rs_;

    Window* window_;
    Input* input_;

    int selected_entity;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    std::vector<std::string> consoleMessages;

    Texture m_DirectoryIcon, m_FileIcon, m_EntityIcon, m_BackIcon,
    m_ToolsIcon, m_LightIcon, m_CameraIcon, m_TransformIcon, m_SettingsIcon,
    m_TagIcon, m_RenderIcon, m_EditIcon, m_PuzzleIcon, m_DeleteIcon,
    m_PlayIcon, m_StopIcon, m_AlgorithmIcon, m_PhysicsmIcon;

    float padding;
    float thumbnailSize;
    
    //--
    bool show_fps;

    bool post_process_window;
    bool skybox_window;
    bool console_window;
    bool fog_window;
    bool l_system_window;
    bool add_mesh_window;
    bool add_surface_window;
    bool add_entities_terrain_window;

    Shader new_pp_shader;
    bool post_process_compiled;
};

#endif // __UI_MANAGER__
