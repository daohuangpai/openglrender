#include "test.h"

#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "../OGL/camera.h"
#include "../OGL/shader.h"
#include <windows.h>
#include "../Graph/Cube.h"
#include "../OGL/vao.h"
#include "../OGL/light.h"
#include "../OGL/util.h"
#include "../tool/path.h"
#include "glftimehelper.h"

//light test
namespace lighttest
{
	GLUI::camera camer;
	static bool rotate = true;
	// Function prototypes
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if ((action == GLFW_RELEASE))
			return;

		if (key == GLFW_KEY_ESCAPE ) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_W) {
			camer.move_front_back(0.1);
		}
		else if (key == GLFW_KEY_R) {
			rotate = !rotate;
		}
		else if (key == GLFW_KEY_S) {
			camer.move_front_back(-0.1);
		}
		else if (key == GLFW_KEY_A) {
			camer.move_left_right(0.1);
		}
		else if (key == GLFW_KEY_D) {
			camer.move_left_right(-0.1);
		}
		else if (key == GLFW_KEY_UP) {
			camer.move_up_down(0.1);
		}
		else if (key == GLFW_KEY_DOWN) {
			camer.move_up_down(-0.1);
		}
		else if (key == GLFW_KEY_LEFT) {
			camer.rotatePosition(0.1);
		}
		else if (key == GLFW_KEY_RIGHT) {
			camer.rotatePosition(-0.1);
		}
		else if (key == GLFW_KEY_Z) {
			camer.rotateEye(0.1);
		}
		else if (key == GLFW_KEY_C) {
			camer.rotateEye(-0.1);
		}

	}

	int main()
	{
		glfwInit();
		// Set all the required options for GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_RESIZABLE, true);

		// Create a GLFWwindow object that we can use for GLFW's functions
		GLFWwindow* window = glfwCreateWindow(800, 600, "testlight", nullptr, nullptr);
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
		glewInit();
		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		std::shared_ptr<GLUI::image> imageR =  GLUI::image::GlobalGet((char*)std::string(PATH::imagepath() + "container2.png").data());


		std::shared_ptr<GLUI::cubeTexture> TextureR(new GLUI::cubeTexture());


		TextureR->bind_data({
			{ GLUI::cubeTexture::RIGHT,(char*)imageR->m_data,imageR->m_width ,imageR->m_height ,imageR->OpengType() },
			{ GLUI::cubeTexture::LEFT,(char*)imageR->m_data,imageR->m_width ,imageR->m_height ,imageR->OpengType() } ,
			{ GLUI::cubeTexture::TOP,(char*)imageR->m_data,imageR->m_width ,imageR->m_height ,imageR->OpengType() } ,
			{ GLUI::cubeTexture::BOTTOM,(char*)imageR->m_data,imageR->m_width ,imageR->m_height ,imageR->OpengType() } ,
			{ GLUI::cubeTexture::FRONT,(char*)imageR->m_data,imageR->m_width ,imageR->m_height ,imageR->OpengType() } ,
			{ GLUI::cubeTexture::BACK,(char*)imageR->m_data,imageR->m_width ,imageR->m_height ,imageR->OpengType() } });


		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());
		GLUI::MakeCubeArrayBufferFront(glm::vec3(1.0, 1.0, 1.0), buffer);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer({ { buffer, sizeof(GLUI::CubePointInfo), 0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, sizeof(GLUI::CubePointInfo), 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT) },
		{ buffer, sizeof(GLUI::CubePointInfo), 2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT) } });

		GLUI::lightsourceobject light[3];

		camer.zNear = 1;
		camer.zFar = -3;
		camer.m_position = glm::vec3(0.9f, 0.0f, -1.6f);
		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)std::string(PATH::shaderpath() + "light_vectex_global_shader.vs").data(),false },
		{ GLUI::shader::FRAGMENT,(char*)std::string(PATH::shaderpath() + "light_fragment_global_shader.gs").data(),false }
		});

		TextureR->Active(0);

		GLUI::lightmodel model[3];

		model[0].enable = 1;
		model[0].directionlight = 0;
		model[0].spotlight = 0;
		model[0].m_spot_direction = glm::vec3(1.0f, 0.0f, 1.0f);
		model[0].m_position = glm::vec3(1.0f, 0.0f, 4.0f);
		model[0].m_ambientlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//环境光，前三个颜色
		model[0].m_diffuselightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//漫反射，前三个颜色
		model[0].m_specularlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//反射光，前三个颜色
		light[0].InitShader();
		light[0].InitVao();
		light[0].setpoint(1.0f, 0.0f, 4.0f);


		model[1].enable = 1;
		model[1].directionlight = 1;
		model[1].spotlight = 1;
		model[1].m_spot_direction = glm::vec3(-5.0f, 0.0f, -4.0f);
		model[1].m_spot_cutoff = 20;
		model[1].m_position = glm::vec3(5.0f, 0.0f, 4.0f);
		model[1].m_ambientlightcolor = glm::vec3(1.0f, 0.0f, 0.0f);//环境光，前三个颜色
		model[1].m_diffuselightcolor = glm::vec3(1.0f, 0.0f, 0.0f);//漫反射，前三个颜色
		model[1].m_specularlightcolor = glm::vec3(1.0f, 0.0f, 0.0f);//反射光，前三个颜色
		light[1].InitShader();
		light[1].lightcolor() = glm::vec3(1.0f, 0.0f, 0.0f);
		light[1].InitVao();
		light[1].setpoint(5.0f, 0.0f, 4.0f);


		model[2].enable = 1;
		model[2].directionlight = 1;
		model[2].spotlight = 1;
		model[2].m_spot_direction = glm::vec3(-5.0f, 0.0f, 4.0f);
		model[2].m_spot_cutoff = 10;
		model[2].m_position = glm::vec3(5.0f, 0.0f, -4.0f);
		model[2].m_ambientlightcolor = glm::vec3(0.0f, 1.0f, 0.0f);//环境光，前三个颜色
		model[2].m_diffuselightcolor = glm::vec3(0.0f, 1.0f, 0.0f);//漫反射，前三个颜色
		model[2].m_specularlightcolor = glm::vec3(0.0f, 1.0f, 0.0f);//反射光，前三个颜色
		light[2].InitShader();
		light[2].lightcolor() = glm::vec3(0.0f, 1.0f, 0.0f);
		light[2].InitVao();
		light[2].setpoint(5.0f, 0.0f, -4.0f);

		GLUI::lightbuffer lightbuffer;
		lightbuffer.init(model, sizeof(model) / sizeof(GLUI::lightmodel));
		lightbuffer.Bind(sdr.get());

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glftimer rottimer;
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			//glfwWaitEvents();
			Sleep(10);
			if (rotate&&rottimer.passsec() > 0.01)
			{
				rottimer.update();
				camer.rotatePosition(-0.05);
			}
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearDepth(1.0);
			glDepthFunc(GL_LEQUAL);
			camer.computematixview();
			camer.computeperspectiveview();
			for (int i = 0; i<sizeof(light) / sizeof(GLUI::lightsourceobject); i++)
				light[i].Drow(camer.m_matixview, camer.m_perspectiveview);
			sdr->Use();
			sdr->set_uniform_int("skybox", 0);
			sdr->set_uniform_float_vec3("eyePos", camer.m_position[0], camer.m_position[1], camer.m_position[2]);
			sdr->set_uniform_mat_4_4("model_matrix", (GLfloat*)&camer.m_matixview[0]);
			sdr->set_uniform_mat_4_4("projection_matrix", (GLfloat*)&camer.m_perspectiveview[0]);
			vao->bind();
			TextureR->bind();
			vao->DrowArray(GLUI::Vao::TRIANGLES, 0, buffer->getsize() / sizeof(GLUI::CubePointInfo));
			opengl_error("hh");
			glfwSwapBuffers(window);
		}
		return 0;
	}
}