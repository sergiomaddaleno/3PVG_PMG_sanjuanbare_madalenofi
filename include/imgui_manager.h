#ifndef IGM
#define IGM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Render/post_processing.h" 

struct RenderData;
class Window;

class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();

    void Init(GLFWwindow* window);
    void Render(GLFWwindow* window, PostProcessing& postProcc);
    void Cleanup();

    // ---

    void ImguiWindow(GLFWwindow* window);
    void GameWindow(PostProcessing& postProcc);
    void Hierarchy();
    void Inspector();
    void ContentBrowser();
    void TopBar();

    //void SetupDocking();
};

#endif // IGM
