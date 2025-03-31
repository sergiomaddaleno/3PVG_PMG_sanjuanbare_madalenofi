#ifndef __ENGINE__
#define __ENGINE__ 1

#include "Managers/ui_manager.h"
#include "Managers/save_manager.h"
#include "Render/physics_system.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"
#include "Render/scripting_system.h"
#include "Geometry/terrain.h"
#include "Render/post_processing.h"
#include "Core/window.h"
#include "Core/input.h"
#include "../src/clouds/CloudsModel.h"
#include "../src/clouds/VolumetricClouds.h"

class Engine {

public:
	Engine(Window& window, Input& input);
	~Engine();

	void init();

	void beginFrame();
	void endFrame();
	void clearWindow();

	Window& window;
	Input& input;

	UIManager ui_manager;
	SaveManager save_manager;

	ECSManager ecs;
	RenderSystem render_system;
	PhysicsSystem physics_system;
	ScriptingSystem scripting_system;

	PostProcessing post_process;

	

	//------------------

	double dt = 0.0;
	bool is_playing = false;

private:
	double lastTime;
	double currentTime;
};

#endif // !__ENGINE__
