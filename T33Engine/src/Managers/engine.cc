#include "Managers/engine.h"

Engine::Engine(Window& window, Input& input) :
	window(window),
	input(input),
	ecs(), 
	render_system(ecs),
	ui_manager(*this, ecs, render_system),
	save_manager(ecs, render_system),
	physics_system(ecs, render_system)
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

	initShader(render_system.shader, "../stages/shaders/forward_shading.vs", "../stages/shaders/forward_shading.fs");
	initShader(render_system.texture_shader, "../stages/shaders/basic/texture.vs", "../stages/shaders/basic/texture.fs");

	initShader(render_system.deferred.gbuffer, "../stages/shaders/g_buffer.vs", "../stages/shaders/g_buffer.fs");
	initShader(render_system.deferred.deferred_shading, "../stages/shaders/deferred_shading.vs", "../stages/shaders/deferred_shading.fs");

	render_system.deferred.init();
	render_system.grid.init();

	//render_system.compileShaderForBuild();
}

void Engine::beginFrame()
{
	currentTime = window.getTime();
	dt = currentTime - lastTime;

	window.updateWindow();
	window.toggleFullScreen(input);

	render_system.resize(window.getWidth(), window.getHeight());

	post_process.resize(window.getWidth(), window.getHeight());
	post_process.beginRender();

	if (render_system.use_deferred_shading) {
		render_system.deferredRender(input, dt);
	}
	else {
		render_system.forwardRender(input, dt);
	}

	physics_system.update();
	physics_system.dt_ = dt;

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
