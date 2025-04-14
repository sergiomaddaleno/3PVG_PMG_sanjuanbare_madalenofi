
#include "Render/raymarch.h"
#include "../include/Components/components.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "utils.h"

RayMarch::RayMarch(ECSManager& ecs) :
    ecs_(ecs),
    window_width(800), window_height(600),
    cube_size_(10.0f),
    absorption_(1.0f, 1.0f, 1.0f),
    march_size_(0.04f),
    speed_(1.0f),
    amplitude_(2.0f),
    frequency_(0.5f),
    cloud_direction_(0.1f, 0.2f, -0.3f),
    max_steps_(30),
    max_steps_light_(6),
    use_blue_noise_(false)
{
}

RayMarch::~RayMarch()
{
}

void RayMarch::initRaymarchShader(std::string vertex_path, std::string fragment_path)
{
    initShader(shader_, vertex_path, fragment_path);
}

void RayMarch::loadNoiseTexture2D(std::string path)
{
    noise_2D_.loadTexture(path);
}

void RayMarch::loadBlueNoiseTexture2D(std::string path)
{
    blue_noise_2D_.loadTexture(path);
}

void RayMarch::initBoundingBox(float size)
{
    cube.init8v(size);
    cube.bind();

    cube_size_ = size;
}

void RayMarch::render(float time)
{
    shader_.useProgram();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float ti = (float)time;
    shader_.setUniformValue(shader_.getUniformLocation("u_time"), ValueType::T_FLOAT_1, &ti);
    int uFrame = int(floor(time * 60.0));
    shader_.setUniformValue(shader_.getUniformLocation("u_frame"), ValueType::T_INT_1, &uFrame);

    int use_blue_noise = use_blue_noise_;
    shader_.setUniformValue(shader_.getUniformLocation("u_use_blue_noise"), ValueType::T_INT_1, &use_blue_noise);

    noise_2D_.active(GL_TEXTURE0);
    noise_2D_.bind2D();
    int tex_id = GL_TEXTURE0;
    shader_.setUniformValue(shader_.getUniformLocation("u_noise_2D"), ValueType::T_INT_1, &tex_id);

    blue_noise_2D_.active(GL_TEXTURE1);
    blue_noise_2D_.bind2D();
    tex_id = GL_TEXTURE1;
    shader_.setUniformValue(shader_.getUniformLocation("u_blue_noise_2D"), ValueType::T_INT_1, &tex_id);

    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();

    for (; ci != ciend; ++ci) {
        if (ci->has_value()) {
            auto& camera_comp = *ci;

            glm::vec3 camera_position = camera_comp->camera.getPosition();
            glm::mat4 view = camera_comp->camera.getViewMatrix();
            glm::mat4 projection = camera_comp->camera.getUsePerspectiveMatrix() ? camera_comp->camera.getPerspectiveMatrix() : camera_comp->camera.getOrthograficMatrix();
            
            auto ri = ecs_.getComponentList<RenderComponent>().begin();
            auto ti = ecs_.getComponentList<TransformComponent>().begin();
            auto riend = ecs_.getComponentList<RenderComponent>().end();
            auto tiend = ecs_.getComponentList<TransformComponent>().end();

            for (; ri != riend && ti != tiend; ++ri, ++ti) {
                if (ri->has_value() && ti->has_value()) {
                    auto& render_comp = *ri;
                    auto& transform_comp = *ti;

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, transform_comp->position);
                    model = model * glm::mat4_cast(transform_comp->rotation);
                    model = glm::scale(model, transform_comp->scale);

                    shader_.setUniformValue(shader_.getUniformLocation("u_cube_scale"), ValueType::T_FLOAT_3, glm::value_ptr(transform_comp->scale));
                    shader_.setUniformValue(shader_.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
                    shader_.setUniformValue(shader_.getUniformLocation("u_camera_pos"), ValueType::T_FLOAT_3, &camera_position);
                    shader_.setUniformValue(shader_.getUniformLocation("view"), ValueType::T_MAT_4x4, &view);
                    shader_.setUniformValue(shader_.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection);

                    shader_.setUniformValue(shader_.getUniformLocation("u_cloud_pos"),ValueType::T_FLOAT_3, glm::value_ptr(transform_comp->position));
                
                    shader_.setUniformValue(shader_.getUniformLocation("u_absorption"), ValueType::T_FLOAT_3, &absorption_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_speed"), ValueType::T_FLOAT_1, &speed_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_march_size"), ValueType::T_FLOAT_1, &march_size_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_max_steps"), ValueType::T_INT_1, &max_steps_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_max_steps_light"), ValueType::T_INT_1, &max_steps_light_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_amplitude"), ValueType::T_FLOAT_1, &amplitude_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_frequency"), ValueType::T_FLOAT_1, &frequency_);
                    shader_.setUniformValue(shader_.getUniformLocation("u_cloud_direction"), ValueType::T_FLOAT_3, &cloud_direction_);
                }
            }

            auto li = ecs_.getComponentList<LightComponent>().begin();
            auto liend = ecs_.getComponentList<LightComponent>().end();

            for (; li != liend; ++li) {
                if (li->has_value()) {
                    auto& light_comp = *li;

                    if (light_comp->light.type_ == Light::LightType::LT_Directional) {

                        shader_.setUniformValue(shader_.getUniformLocation("u_light_color"), ValueType::T_FLOAT_3, &light_comp->light.light_color_);
                        shader_.setUniformValue(shader_.getUniformLocation("u_sun_dir"), ValueType::T_FLOAT_3, &light_comp->light.dir_);
                    }
                }
            }
            cube.draw();
        }
    }

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); 
    glDisable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void RayMarch::resize(unsigned int width, unsigned int height)
{
    if (window_width != width || window_height != height) {

        window_width = width;
        window_height = height;
    }
}

nlohmann::json RayMarch::serialize() const
{
    nlohmann::json json_data;

        json_data["cube_size"] = cube_size_;
        json_data["absorption"] = { absorption_.x, absorption_.y, absorption_.z };
        json_data["march_size_"] = march_size_;
        json_data["speed"] = speed_;
        json_data["amplitude"] = amplitude_;
        json_data["frequency"] = frequency_;
        json_data["cloud_direction"] = { cloud_direction_.x, cloud_direction_.y, cloud_direction_.z};
        json_data["max_steps"] = max_steps_;
        json_data["max_steps_light_"] = max_steps_light_;
        json_data["use_blue_noise"] = use_blue_noise_;

    return json_data;
}

