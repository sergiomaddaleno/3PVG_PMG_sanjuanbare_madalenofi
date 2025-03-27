/**
 * @file globals.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __GLOBALS__
#define __GLOBALS__

#include "Core/window.h"
#include "Core/camera.h"
#include "Render/post_processing.h"
#include "Render/skybox.h"
#include "Render/render_system.h"
#include "Render/physics_system.h"
#include "Systems/ecs_manager.h"
#include "Managers/save_manager.h"
#include "Managers/ui_manager.h"
#include "Geometry/terrain.h"

 /**
  * @class Globals
  * @brief Manages global variables and objects for the application.
  */
class Globals {
public:

    Globals();

    //Variables
    int window_width = 800;
    int window_height = 600;
    bool full_screen = true;
    double dt = 0.0;

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

    bool deferred_shading = false;
    int deferred_option = 0;

    float alpha = 1.0f;
    float alpha_blend = 0.8f;

    // -------------------------------------------------------------
    
    //Classes
    ECSManager ecs;
    RenderSystem render_system;
    PhysicsSystem physics_system;
    PostProcessing post_process;
    Skybox skybox;
    SaveManager save_manager;
    UIManager ui_manager;
    Terrain grid;
};

extern Globals global;

#endif // !__GLOBALS__
