#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "Core/window.h"
#include "Render/camera.h"
#include "Render/post_processing.h"
#include "Render/skybox.h"
#include "Render/render_system.h"
#include "Systems/ecs_manager.h"
#include "Managers/save_manager.h"
#include "Managers/imgui_manager.h"
#include "Geometry/terrain.h"

class Globals {
public:

    //Variables
    int window_width = 800;
    int window_height = 600;
    bool full_screen = true;

    float ortho_width = 45.0f;
    float ortho_height = 45.0f;

    float fov = 45.0f;
    float near_clip_plane = 0.1f;
    float far_clip_plane = 100.0f;

    bool use_fog = false;
    float fog_density = 0.1f;
    float fog_gradient = 1.0f;
    glm::vec3 sky_colour = glm::vec3(0.5f, 0.5f, 0.5f);

    bool use_skybox = true;
    bool is_playing = false;

    // -------------------------------------------------------------
    
    //Classes
    Camera cam;
    ECSManager ecs;
    PostProcessing post_process;
    RenderSystem render_system;
    Skybox skybox;
    SaveManager save_manager;
    ImGuiManager imgui_manager;
    Terrain grid;
};

extern Globals global;

#endif // _GLOBALS_H_
