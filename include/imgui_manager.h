#ifndef IGM
#define IGM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "post_processing.h" 

struct RenderData;

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window);
    void Render(GLFWwindow* window, RenderData& rd, PostProcessing& postProcc);
    void Cleanup();

    // ---

    void Hierarchy();
    void Inspector();
    void ContentBrowser();
    void TopBar();
    void GameWindow(RenderData& rd, PostProcessing& postProcc);

    void SetupDocking();
};

#endif // IGM
