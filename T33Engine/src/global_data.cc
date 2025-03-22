#include "global_data.h"

Globals::Globals() : ecs(), render_system(ecs), 
ui_manager(ecs, render_system),
save_manager(ecs, render_system),
physics_system(ecs, render_system)
{
}

Globals global;
