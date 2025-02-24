#ifndef __LUA_MANAGER__
#define __LUA_MANAGER__

#include "../Systems/scripting.h"

class LuaManager {

public:
    void loadFile(const char* lua_file);

    struct Window {
        int width;
        int height;
        bool full_screen;
    } window;

    struct Camera {
        float fov;
        float min_render_distance;
        float max_render_distance;
    } camera;

    struct Options {
        bool fog;
    } options;


private:
    LuaScript ls;
};
#endif // !__LUA_MANAGER__
