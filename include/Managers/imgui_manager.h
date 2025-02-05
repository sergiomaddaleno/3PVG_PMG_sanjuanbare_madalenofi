#ifndef IGM
#define IGM

#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Core/window.h"
#include "Core/input.h"
#include "Core/texture.h"
#include "Core/shader.h"
#include "Core/framebuffer.h"

#include "Render/camera.h"
#include "Render/post_processing.h"
#include "Render/skybox.h"
#include "Render/light.h"
#include "Render/render_system.h"

#include "Geometry/terrain.h"
#include "Geometry/triangle.h"

#include "Systems/ecs_manager.h"
#include "save_manager.h"
#include "utils.h"


//Todo lo que quiero que se edite en imgui
struct DataImgui {
    //Terrain terrain;
    //Triangle triangle;
    Camera cam;
    //LightInfo light_info;
    PostProcessing* post_proccessing;
    ECSManager* ecs;
    RenderSystem* render_system;
    Shader shader;
    Window* window;
    Input* input;
    Skybox skybox;
    SaveManager save_manager;
};

static DataImgui data_imgui;

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window);
    void Render(GLFWwindow* window, DataImgui &di);
    void Cleanup();

    void SetImguiTheme();

    // ---

    void ImguiWindow(GLFWwindow* window);
    void GameWindow(DataImgui& di);
    void HierarchyWindow(DataImgui& di);
    void InspectorWindow(DataImgui& di);
    void ContentBrowserWindow(DataImgui& di);
    void TopBarWindow(DataImgui& di);

    void PostProcessWindow(DataImgui& di);
    void FogWindow(DataImgui& di);
    void SkyboxWindow(DataImgui& di);
    void ConsoleWindow(DataImgui& di);
    void LoadSceneWindow(DataImgui& di);

    void Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity, DataImgui& di);

private:
    int selected_entity = -1;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    Texture m_DirectoryIcon;
    Texture m_FileIcon;
    Texture m_EntityIcon;
    Texture m_EntityLightBulbIcon;

    bool post_process_window = false;
    bool skybox_window = false;
    bool console_window = false;
    bool fog_window = false;

    Shader new_pp_shader;
    Shader grey_sh, negative_sh, waves_sh, blur_sh, kernel_sh;
    bool post_process_compiled = false;
    Effect current_effect = Effect::None;
    bool none_render = false;

    bool load_scene = false;
};

#endif // IGM
