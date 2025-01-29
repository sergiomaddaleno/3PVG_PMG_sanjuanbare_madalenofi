#ifndef GLOBALS_H
#define GLOBALS_H

#include <../deps/glm/glm.hpp>

extern int g_window_width_;
extern int g_window_height_;
extern bool g_full_screen_;

extern float g_fov_;
extern float g_min_render_distance_;
extern float g_max_render_distance_;

extern bool g_b_fog_;
extern float g_fog_density_;
extern float g_fog_gradient_;
extern glm::vec3 g_sky_colour_;


#endif // GLOBALS_H