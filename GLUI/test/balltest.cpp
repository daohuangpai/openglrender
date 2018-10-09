#include "test.h"
#include "../OGL/shader.h"
#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "../OGL/camera.h"
#include <windows.h>
#include "../OGL/vao.h"
#include "../tool/path.h"
#include "../Graph/ball.h"
#include "glftimehelper.h"

namespace balltest
{

		GLUI::camera camer;
		// Function prototypes
		void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GL_TRUE);
			}
			else if (key == GLFW_KEY_W) {
				camer.move_front_back(0.1f);
			}
			else if (key == GLFW_KEY_S) {
				camer.move_front_back(-0.1f);
			}
			else if (key == GLFW_KEY_A) {
				camer.move_left_right(0.1f);
			}
			else if (key == GLFW_KEY_D) {
				camer.move_left_right(-0.1f);
			}
			else if (key == GLFW_KEY_UP) {
				camer.move_up_down(-0.1f);
			}
			else if (key == GLFW_KEY_DOWN) {
				camer.move_up_down(0.1f);
			}
			else if (key == GLFW_KEY_LEFT) {
				camer.rotatePosition(0.1f);
			}
			else if (key == GLFW_KEY_RIGHT) {
				camer.rotatePosition(-0.1f);
			}
			else if (key == GLFW_KEY_Z) {
				camer.rotateEye(0.1f);
			}
			else if (key == GLFW_KEY_C) {
				camer.rotateEye(-0.1f);
			}

		}
		enum {
			LEFT_BUTTON = 0x0004,
			MIDDLE_BUTTON = 0x0008,
			RIGHT_BUTTON = 0x0010
		};
		bool statue = false;
		glm::vec2 oldpoint = { 0,0 };
		void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
		{
			if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
				POINT point;
				GetCursorPos(&point);
				oldpoint = glm::vec2(point.x, point.y);
				statue = true;
			}

			if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_RELEASE))
				statue = false;
		}


		void cursor_position_callback(GLFWwindow* window, double x, double y)
		{
			if (statue)
			{
				glm::vec2 offset = glm::vec2(x - oldpoint[0], y - oldpoint[1]);
				camer.rotateEye(0.005*offset[0]);
				oldpoint = glm::vec2(x, y);
			}
		}

		// Window dimensions
		const GLuint WIDTH = 800, HEIGHT = 600;


		GLfloat vertices[] = {
			-0.5f, -0.5f, 0.0f,0.0f,1.0f,
			-0.5f, 0.5f, 0.0f,0.0f,0.0f,
			0.5f, -0.5f, 0.0f,1.0f,1.0f,
			0.5f, 0.5f, 0.0f,1.0f,0.0f
		};


		//下面例子有原子缓冲区的操作
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
			GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "testball", nullptr, nullptr);
			glfwSetKeyCallback(window, key_callback);
			glfwSetMouseButtonCallback(window, mouse_button_callback);
			glfwSetCursorPosCallback(window, cursor_position_callback);
			glfwMakeContextCurrent(window);
			glewInit();
			// Define the viewport dimensions
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);

			std::shared_ptr<GLUI::image> imageR = GLUI::image::GlobalGet((PATH::imagepath() + "W020110211543376953862.jpg").data());//"C:/Users/me/Desktop/all database/12425820_1.jpg"

			std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());

			TextureR->Height = imageR->m_height;
			TextureR->Width = imageR->m_width;
			switch (imageR->m_channle)
			{
			case 1:TextureR->setInternalFormat(GL_LUMINANCE); break;
			case 3:TextureR->setInternalFormat(GL_RGB);  break;
			case 4:TextureR->setInternalFormat(GL_RGBA); break;
			}

			TextureR->bind_data((char*)imageR->m_data);


			std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());
			GLUI::generate_ArrayBuffer(buffer, 0.2);
			std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

			vao->BindBuffer( { { buffer, 8 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
			{ buffer, 8 * sizeof(GL_FLOAT),1,2,GL_FLOAT,GL_FALSE,6 * sizeof(GL_FLOAT) } });


			camer.zNear = 0;
			camer.zFar = 3;
			camer.m_position = glm::vec3(3.0f, 0.0f, -2.0f);
			std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
			sdr->createshader({ { GLUI::shader::VERTEX,(char*)std::string(PATH::shaderpath() + "ball_vectex_shader.vs").data(),false },
			                    { GLUI::shader::FRAGMENT,(char*)std::string(PATH::shaderpath() + "ball_fragment_shader.gs").data(),false }
			});

			TextureR->Active(0);

			glEnable(GL_CULL_FACE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			bool use = true;
			bool useline = true;
			glftimer rottimer;
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
				Sleep(10);

				if (rottimer.passsec() > 0.01)
				{
					rottimer.update();
					camer.rotatePosition(-0.05f);
				}
				//glfwWaitEvents();
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				camer.computematixview();
				camer.computeperspectiveview();
				sdr->Use();
				sdr->set_uniform_int("skybox", 0);
				sdr->set_uniform_mat_4_4("model_matrix", (GLfloat*)&camer.m_matixview[0]);
				sdr->set_uniform_mat_4_4("projection_matrix", (GLfloat*)&camer.m_perspectiveview[0]);
				vao->bind();
				opengl_error("hh");
				TextureR->bind();
				opengl_error("hh");
				vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, buffer->getsize() / 32);
				opengl_error("hh");
				glfwSwapBuffers(window);
			}
			glfwTerminate();
			return 0;
		}
	
}