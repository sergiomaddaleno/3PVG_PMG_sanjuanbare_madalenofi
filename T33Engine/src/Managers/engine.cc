#include "Managers/engine.h"

Engine::Engine(Window& window, Input& input) :
	window(window),
	input(input),
	ecs(), 
	render_system(ecs),
	ui_manager(*this, ecs, render_system),
	save_manager(ecs, render_system),
	physics_system(ecs, render_system),
	raymarch(ecs),
	scripting_system(ecs),
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
	render_system.grid.initGrid();

	raymarch.init();

	//render_system.compileShaderForBuild();
}

void Engine::beginFrame()
{
	currentTime = window.getTime();
	dt = currentTime - lastTime;

	window.updateWindow();
	window.toggleFullScreen(input);

	post_process.resize(window.getWidth(), window.getHeight());
	post_process.beginRender();

	render_system.resize(window.getWidth(), window.getHeight());
	if (render_system.use_deferred_shading) {
		render_system.deferredRender(input, dt);
	}
	else {
		render_system.forwardRender(input, dt);
	}

	physics_system.update((float)dt);
	scripting_system.update((float)dt);

	raymarch.resize(window.getWidth(), window.getHeight());
	//raymarch.render((float)window.getTime());

	ui_manager.Render();

	lastTime = currentTime;
}

void Engine::endFrame()
{
	lastTime = currentTime;
}

void Engine::clearWindow()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
