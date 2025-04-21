#include "Managers/engine.h"
#include "Managers/resource_manager.h"

Engine::Engine(Window& window, Input& input) :
	window(window),
	input(input),
	ecs(), 
	render_system(ecs),
	ui_manager(*this, ecs, render_system),
	physics_system(ecs, render_system),
	scripting_system(ecs),
	save_manager(ecs, render_system, scripting_system),
	raymarch(ecs),
	lastTime(0.0),
	currentTime(0.0)
{
}

Engine::~Engine()
{

}

void Engine::init()
{
	window.setWindowActive();
	window.initGLEW();
	window.setResizeCallback();

	post_process.initData();
	render_system.setupEntities();
	ui_manager.Init(&window, &input);

	render_system.initshaders();
	render_system.deferred.init();
	render_system.ssao.init();
	render_system.grid.initGrid();

	ecs.remove_entities();
	render_system.entity_ = 0;
	render_system.entity_counts_ = {};
	ResourceManager::GetInstance().clearMaps();
}

void Engine::beginFrame()
{
    currentTime = window.getTime();
    dt = currentTime - lastTime;

    window.updateWindow();
    window.toggleFullScreen(input);

    unsigned int width = window.getWidth();
    unsigned int height = window.getHeight();

    post_process.resize(width, height);
    post_process.beginRender();

    // --- Render scene
    render_system.resize(width, height);
	if (render_system.use_deferred_shading) {
        render_system.deferredRender(input, dt);
	}
	else {
        render_system.forwardRender(input, dt);
	}
	//render_system.SSAORender(input, dt);

	if (is_playing) {
		physics_system.update((float)dt);
		scripting_system.update((float)dt);
	}

    // --- 

    ui_manager.BeginFrame();
    ui_manager.Render();

    post_process.endRender();

    ui_manager.GameWindow();
    ui_manager.EndFrame();
}

void Engine::endFrame()
{
	lastTime = currentTime;
}

void Engine::clearWindow()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
