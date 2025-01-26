#ifndef IGM
#define IGM

#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "input.h"
#include "Render/camera.h"
#include "Render/post_processing.h"
#include "Render/skybox.h"
#include "Render/light.h"
#include "Render/render_system.h"
#include "Render/shader.h"
#include "Geometry/terrain.h"
#include "Geometry/triangle.h"
#include "Systems/ecs_manager.h"
#include "utils.h"
#include "framebuffer.h"
#include "save_manager.h"
#include "Render/texture.h"

//Todo lo que quiero que se edite en imgui
struct DataImgui {
    //Terrain terrain;
    //Triangle triangle;
    Camera cam;
    //LightInfo light_info;
    PostProcessing post_proccessing;
    ECSManager* ecs;
    RenderSystem* render_system;
    Shader* shader;
    Window* window;
    Input* input;
    //Skybox sxybox;
    GLuint game_texture;

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

    void Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity, DataImgui& di);

private:
    int selected_entity = -1;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    Texture m_DirectoryIcon;
    Texture m_FileIcon;
    Texture m_EntityIcon;

};

#endif // IGM
