#include "Managers/lua_manager.h"


void LuaManager::loadFile(const char* lua_file)
{
	ls.run(lua_file);

	ls.get_table("Window");
	window.width = ls.get_integer("window_width");
	window.height = ls.get_integer("window_height");
	window.full_screen = ls.get_bool("full_screen");
	ls.pop();

	ls.get_table("Camera");
	camera.fov = ls.get_float("fov");
	camera.min_render_distance = ls.get_float("fov");
	camera.max_render_distance = ls.get_float("fov");
	ls.pop();

	ls.get_table("Options");
	options.fog = ls.get_bool("fog");
	ls.pop();

}
