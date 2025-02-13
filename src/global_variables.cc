#include "global_variables.h"

int g_window_width_ = 800;
int g_window_height_ = 600;
bool g_full_screen_ = true;

float g_ortho_width_ = 800;
float g_ortho_height_ = 600;

float g_fov_ = 45.0f;
float g_near_clip_plane_ = 0.1f;
float g_far_clip_plane_ = 400.0f;

bool g_b_fog_ = false;
float g_fog_density_ = 0.06f;
float g_fog_gradient_ = 2.0f;
glm::vec3 g_sky_colour_ = {0.5f, 0.5f, 0.5f};

bool g_use_skybox_ = true;
