#ifndef IGM
#define IGM

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"
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

//Todo lo que quiero que se edite en imgui
struct DataImgui {
    //Terrain terrain;
    //Triangle triangle;
    Camera cam;
    //LightInfo light_info;
    PostProcessing post_proccessing;
    ECSManager* ecs;
    RenderSystem render_system;
    Shader shader;
    //Skybox sxybox;
    GLuint game_texture;
};

static DataImgui data_imgui;

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window);
    void Render(GLFWwindow* window, DataImgui &di);
    void Cleanup();

    // ---

    void ImguiWindow(GLFWwindow* window);
    void GameWindow(DataImgui& di);
    void Hierarchy(DataImgui& di);
    void Inspector(DataImgui& di);
    void ContentBrowser(DataImgui& di);
    void TopBar(DataImgui& di);

    void Gizmo(const glm::mat4& view, const glm::mat4& projection, int selected_entity, DataImgui& di);

private:
    int selected_entity = -1;
};

#endif // IGM
