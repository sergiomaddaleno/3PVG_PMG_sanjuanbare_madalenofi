
#include "Render/ssao.h"
#include "../include/Components/components.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "utils.h"

SSAO::SSAO(ECSManager& ecs) : ssaoFBO(0),
ssaoColorBuffer(0), noiseTexture(0), width_(800), height_(600),randomFloats(0.0,1.0),
gBuffer(0), rboDepth(0),
gPosition(0), gNormal(0), gAlbedoSpec(0),
ecs_(ecs), ssao_option(0),
kernel_size(64), radius(0.5f), bias(0.025f)
{
}

SSAO::~SSAO()
{
}

void SSAO::init()
{
    quad.initVec3();
    quad.bindVec3();

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    //-----------------------------------------------------------------------------

    glGenFramebuffers(1, &ssaoFBO); 
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Kernel
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        scale = std::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // Noise
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, glm::value_ptr(ssaoNoise[0]));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

glm::mat4 SSAO::getWorldTransform(int entity)
{
    if (!ecs_.has_component<TransformComponent>(entity))
        return glm::mat4(1.0f);

    auto& transform = ecs_.get_component<TransformComponent>(entity);
    glm::mat4 local = glm::translate(glm::mat4(1.0f), transform->position) *
        glm::mat4_cast(transform->rotation) *
        glm::scale(glm::mat4(1.0f), transform->scale);

    if (ecs_.has_component<HierarchyComponent>(entity)) {
        auto& hierarchy = ecs_.get_component<HierarchyComponent>(entity);
        if (hierarchy->hasParent()) {
            int parent = hierarchy->getParent();
            return getWorldTransform(parent) * local;
        }
    }

    return local;
}

void SSAO::render(Camera cam)
{
    // Geometry pass -------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    geometry.useProgram();

    auto ri = ecs_.getComponentList<RenderComponent>().begin();
    auto ti = ecs_.getComponentList<TransformComponent>().begin();
    auto riend = ecs_.getComponentList<RenderComponent>().end();
    auto tiend = ecs_.getComponentList<TransformComponent>().end();

    int entity = 0;
    for (; ri != riend && ti != tiend; ++ri, ++ti, ++entity) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            glm::mat4 model = this->getWorldTransform(entity);
            glm::mat4 view = cam.getViewMatrix();
            glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

            geometry.setUniformValue(geometry.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
            geometry.setUniformValue(geometry.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
            geometry.setUniformValue(geometry.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

            int normals_inv = 0;
            geometry.setUniformValue(geometry.getUniformLocation("invertedNormals"), ValueType::T_INT_1, &normals_inv);

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
                for (auto& mesh : render_comp->mesh_->meshes) {
                    render_comp->mesh_->draw();
                }
                break;
            case RenderType::RT_Terrain:
                render_comp->terrain_->draw();
                break;
            case RenderType::RT_Sphere:
                render_comp->sphere_->draw();
                break;
            case RenderType::RT_Surface:
                render_comp->surface_->draw();
                break;
            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v:
                render_comp->cube_->draw();
                break;
            case RenderType::RT_Quad:
                render_comp->quad_->draw();
                break;
            case RenderType::RT_Water:
                render_comp->water_->draw();
                break;
            }
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 1);

    // SSAO texture ----------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.useProgram();

    for (unsigned int i = 0; i < 64; ++i)
        shader.setUniformValue(shader.getUniformLocation(("samples[" + std::to_string(i) + "]").c_str()), ValueType::T_FLOAT_3, &ssaoKernel[i]);

    glm::mat4 model = this->getWorldTransform(entity);
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));
    
    shader.setUniformValue(shader.getUniformLocation("u_kernel_size"), ValueType::T_INT_1, &kernel_size);
    shader.setUniformValue(shader.getUniformLocation("u_radius"), ValueType::T_FLOAT_1, &radius);
    shader.setUniformValue(shader.getUniformLocation("u_bias"), ValueType::T_FLOAT_1, &bias);

    // generate ssao Textures ----------------------------------------
    glActiveTexture(GL_TEXTURE0 + TI_Position);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE0 + TI_Normal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE0 + TI_Noise);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    

    int tex = TI_Position;
    shader.setUniformValue(shader.getUniformLocation("gPosition"), ValueType::T_INT_1, &tex);
    tex = TI_Normal;
    shader.setUniformValue(shader.getUniformLocation("gNormal"), ValueType::T_INT_1, &tex);
    tex = TI_Noise;
    shader.setUniformValue(shader.getUniformLocation("texNoise"), ValueType::T_INT_1, &tex);

    quad.drawVec3();
    glBindFramebuffer(GL_FRAMEBUFFER, 1);

    //Lighting pass ----------------------------------------
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    light_pass.useProgram();

    glActiveTexture(GL_TEXTURE0 + TI_Position);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE0 + TI_Normal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE0 + TI_Albedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glActiveTexture(GL_TEXTURE0 + TI_SSAO);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

    int tex_id = TI_Position;
    light_pass.setUniformValue(light_pass.getUniformLocation("gPosition"), ValueType::T_INT_1, &tex_id);
    tex_id = TI_Normal;
    light_pass.setUniformValue(light_pass.getUniformLocation("gNormal"), ValueType::T_INT_1, &tex_id);
    tex_id = TI_Albedo;
    light_pass.setUniformValue(light_pass.getUniformLocation("gAlbedoSpec"), ValueType::T_INT_1, &tex_id);
    tex = TI_SSAO;
    light_pass.setUniformValue(light_pass.getUniformLocation("ssaoTex"), ValueType::T_INT_1, &tex);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthFunc(GL_LEQUAL);

    auto li = ecs_.getComponentList<LightComponent>().begin();
    auto liend = ecs_.getComponentList<LightComponent>().end();
    for (; li != liend; ++li) {
       if (li->has_value()) {
            auto& light_comp = *li;
            //pass shadow
            bool useDepthMap = true;
            int useDepthMapInt = useDepthMap ? 1 : 0;
            light_pass.setUniformValue(light_pass.getUniformLocation("u_useDepthMap"), ValueType::T_INT_1, &useDepthMapInt);

            light_comp->shadow_map_->render(light_pass, *light_comp);

            //pass light
            useDepthMap = false;
            useDepthMapInt = useDepthMap ? 1 : 0;

            light_pass.setUniformValue(light_pass.getUniformLocation("u_useDepthMap"), ValueType::T_INT_1, &useDepthMapInt);
            light_comp->light.setUniforms(light_pass);
            light_pass.setUniformValue(light_pass.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));

            light_pass.setUniformValue(light_pass.getUniformLocation("u_ssao_option"), ValueType::T_INT_1, &ssao_option);
            light_pass.setUniformValue(light_pass.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));

            glm::mat4 model = this->getWorldTransform(entity);
            glm::mat4 view = cam.getViewMatrix();
            glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

            shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
            shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
            shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

            quad.drawVec3();
       }
        glBlendFunc(GL_ONE, GL_ONE);
    }

    glDisable(GL_BLEND);
    glUseProgram(0);

    // ----------------------------------------------------------------------------------

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 1);

    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 1);
}

void SSAO::resize(unsigned int width, unsigned int height)
{
    if (width_ != width || height_ != height) {
        width_ = width;
        height_ = height;

        init();
    }
}
