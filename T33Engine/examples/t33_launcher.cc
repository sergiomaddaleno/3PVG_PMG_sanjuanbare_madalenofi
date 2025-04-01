#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>
#include "../src/clouds/VolumetricClouds.h"
#include "Managers/engine.h"
#include "Core/window.h"
#include "Core/input.h"
#include "utils.h"

int main(int, char**) {

    auto window = Window::make(800, 600, "T33Engine", false);
    Input input{ window->getWindow() };

    Engine T33Engine{*window, input};

    T33Engine.init();

    CloudsModel cm{};
    VolumetricClouds volumetricClouds(800, 600, &cm);
    VolumetricClouds reflectionVolumetricClouds(1280, 720, &cm);

    T33Engine.save_manager.loadFromFile("../stages/saves/main_scene.json");

	ScreenSpaceShader PostProcessing_("../src/clouds/post_processing.frag");
	ScreenSpaceShader fboVisualizer("../src/clouds/visualizeFbo.frag");



    bool done = false;
    while (!done) {

	

        T33Engine.beginFrame();
        
        Shader2& post = PostProcessing_.getShader();



        ScreenSpaceShader::disableTests();

        volumetricClouds.draw(T33Engine);
        unbindCurrentFrameBuffer();


        post.use();
        post.setVec2("resolution", glm::vec2(800, 600));

        PostProcessing_.draw();

        T33Engine.endFrame();
        if (window->closePressed(input)) done = true;
    }

    return 0;
}





