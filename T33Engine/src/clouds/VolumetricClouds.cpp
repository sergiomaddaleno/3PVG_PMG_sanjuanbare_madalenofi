#include "VolumetricClouds.h"

#define TIMETO(CODE, TASK) 	t1 = glfwGetTime(); CODE; t2 = glfwGetTime(); std::cout << "Time to " + std::string(TASK) + " :" << (t2 - t1)*1e3 << "ms" << std::endl;



VolumetricClouds::VolumetricClouds(int SW, int SH, CloudsModel * model): SCR_WIDTH(SW), SCR_HEIGHT(SH), model(model) {

	cloudsFBO = new TextureSet(SW, SH, 4);
	cloudsPostProcessingFBO = new FrameBufferObject(SW, SH, 2);

}


void VolumetricClouds::draw(Engine& s) {

	float t1, t2;

	//cloudsFBO->bind();
	for (int i = 0; i < cloudsFBO->getNTextures(); ++i) {
		bindTexture2D(cloudsFBO->getColorAttachmentTex(i), i);
	}

	//Shader & cloudsShader = volumetricCloudsShader->getShader();
	Shader2& cloudsShader = *model->volumetricCloudsShader;


	auto ci = s.ecs.getComponentList<CameraComponent>().begin();
	auto ciend = s.ecs.getComponentList<CameraComponent>().end();

	for (; ci != ciend; ++ci) {
		if (ci->has_value()) {
			auto& camera_comp = *ci;


			cloudsShader.use();

			glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

			cloudsShader.setVec2("iResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
			cloudsShader.setFloat("iTime", glfwGetTime());
			cloudsShader.setMat4("inv_proj", glm::inverse(camera_comp->camera.getPerspectiveMatrix()));
			cloudsShader.setMat4("inv_view", glm::inverse(camera_comp->camera.getViewMatrix()));
			cloudsShader.setVec3("cameraPosition", camera_comp->camera.getPosition());
			cloudsShader.setFloat("FOV", camera_comp->camera.getFov());
			cloudsShader.setVec3("lightDirection", glm::normalize(lightPos - camera_comp->camera.getPosition()));
			cloudsShader.setVec3("lightColor", lightColor);

			cloudsShader.setFloat("coverage_multiplier", model->coverage);
			cloudsShader.setFloat("cloudSpeed", model->cloudSpeed);
			cloudsShader.setFloat("crispiness", model->crispiness);
			cloudsShader.setFloat("curliness", model->curliness);
			cloudsShader.setFloat("absorption", model->absorption * 0.01);
			cloudsShader.setFloat("densityFactor", model->density);

			//cloudsShader.setBool("enablePowder", enablePowder);

			cloudsShader.setFloat("earthRadius", model->earthRadius);
			cloudsShader.setFloat("sphereInnerRadius", model->sphereInnerRadius);
			cloudsShader.setFloat("sphereOuterRadius", model->sphereOuterRadius);

			cloudsShader.setVec3("cloudColorTop", model->cloudColorTop);
			cloudsShader.setVec3("cloudColorBottom", model->cloudColorBottom);

			glm::mat4 vp = camera_comp->camera.getPerspectiveMatrix() * camera_comp->camera.getViewMatrix();
			cloudsShader.setMat4("invViewProj", glm::inverse(vp));
			cloudsShader.setMat4("gVP", vp);

			cloudsShader.setSampler3D("cloud", model->perlinTex, 0);
			cloudsShader.setSampler3D("worley32", model->worley32, 1);
			cloudsShader.setSampler2D("weatherTex", model->weatherTex, 2);
			cloudsShader.setSampler2D("depthMap", TextureID::TI_ShadowMap, 3);





			//actual draw
			//volumetricCloudsShader->draw();

			glDispatchCompute(INT_CEIL(SCR_WIDTH, 16), INT_CEIL(SCR_HEIGHT, 16), 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


			//copy to lastFrameFBO

			if (model->postProcess) {
				// cloud post processing filtering
				cloudsPostProcessingFBO->bind();
				Shader2& cloudsPPShader = model->postProcessingShader->getShader();

				cloudsPPShader.use();

				cloudsPPShader.setSampler2D("clouds", cloudsFBO->getColorAttachmentTex(VolumetricClouds::fragColor), 0);
				cloudsPPShader.setSampler2D("emissions", cloudsFBO->getColorAttachmentTex(VolumetricClouds::bloom), 1);
				cloudsPPShader.setSampler2D("depthMap", TextureID::TI_ShadowMap, 2);

				cloudsPPShader.setVec2("cloudRenderResolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
				cloudsPPShader.setVec2("resolution", glm::vec2(800, 600)); //cambiar

				glm::mat4 lightModel;
				lightModel = glm::translate(lightModel, lightPos);
				glm::vec4 pos = vp * lightModel * glm::vec4(0.0, 60.0, 0.0, 1.0);
				pos = pos / pos.w;
				pos = pos * 0.5f + 0.5f;

				//std::cout << pos.x << ": X; " << pos.y << " Y;" << std::endl;
				cloudsPPShader.setVec4("lightPos", pos);

				bool isLightInFront = false;
				float lightDotCameraFront = glm::dot(glm::normalize(lightPos - camera_comp->camera.getPosition()), glm::normalize(camera_comp->camera.getViewDir()));
				//std::cout << "light dot camera front= " << lightDotCameraFront << std::endl;
				if (lightDotCameraFront > 0.2) {
					isLightInFront = true;
				}

				cloudsPPShader.setBool("isLightInFront", isLightInFront);
				cloudsPPShader.setBool("enableGodRays", model->enableGodRays);
				cloudsPPShader.setFloat("lightDotCameraFront", lightDotCameraFront);

				cloudsPPShader.setFloat("time", glfwGetTime());

				model->postProcessingShader->draw();
			}

		}
	}
}




VolumetricClouds::~VolumetricClouds()
{
}
