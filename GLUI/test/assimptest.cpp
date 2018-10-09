#include "test.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "../OGL/camera.h"
#include "../tool/path.h"
#include "../Graph/ball.h"
#include "glftimehelper.h"
#include "../model/model.h"
#include "..\model\render.h"
#include<boost/timer.hpp> 
#include "../model/dataarray.h"
//light test
namespace assimptest
{

	float speed = 0.1f;
	GLUI::camera camer;
	// Function prototypes
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_T && action == GLFW_PRESS) {
			camer.log();
		}
		else if (key == GLFW_KEY_W) {
			camer.move_front_back(speed);
		}
		else if (key == GLFW_KEY_S) {
			camer.move_front_back(-speed);
		}
		else if (key == GLFW_KEY_A) {
			camer.move_left_right(speed);
		}
		else if (key == GLFW_KEY_D) {
			camer.move_left_right(-speed);
		}
		else if (key == GLFW_KEY_UP) {
			camer.move_up_down(-speed);
		}
		else if (key == GLFW_KEY_DOWN) {
			camer.move_up_down(speed);
		}
		else if (key == GLFW_KEY_LEFT) {
			camer.rotatePosition(speed);
		}
		else if (key == GLFW_KEY_RIGHT) {
			camer.rotatePosition(-speed);
		}
		else if (key == GLFW_KEY_Z) {
			camer.rotateEye(speed);
		}
		else if (key == GLFW_KEY_C) {
			camer.rotateEye(-speed);
		}

	}



	int main()
	{
		glfwInit();
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		//glfwWindowHint(GLFW_RESIZABLE, true);


		// Create a GLFWwindow object that we can use for GLFW's functions
		GLFWwindow* window = glfwCreateWindow(1024, 1024, "testassimp", nullptr, nullptr);
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
		glewInit();
		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		MODEL::model assimploader;
		MODEL::meshrender render;
		render.init();
		bool ret = assimploader.load("test.md5mesh");
		assert(ret);
		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)std::string(PATH::shaderpath() + "assmodel_vectex_shader.vs").data(),false },
		{ GLUI::shader::FRAGMENT,(char*)std::string(PATH::shaderpath() + "assmodel_geometry_point_shader.gs").data(),false }
		});
		//glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		bool use = true;
		bool useline = true;
		glftimer rottimer;
		camer.zNear = 1;
		camer.zFar = -3;
		camer.m_position = glm::vec3(0, -5.499997, -10.194863);
		camer.m_lookat_position = glm::vec3(0.0, 0, 0);
		camer.m_up = glm::vec3(0.0f, -1.0f, 0.0f);
		float v[] = { 1.0f,0.0f,0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		glm::mat4 mat = glm::make_mat4(v);
		MODEL::AABB aabb = assimploader.get_rootnode()->get_AABB(&assimploader);
		glm::vec3 size = aabb.size();
		float scale =10.0f / max(max(size.x, size.y), size.z);
		glm::vec3 middele = aabb.min + size / 2.0f;
		mat = glm::translate(mat, -middele);
		//mat = glm::translate(mat, glm::vec3(-5.0f, 0.0f, 0.0f));
		mat = glm::scale(glm::vec3(scale, scale, scale))*mat;
		

		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		boost::timer ter;
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			Sleep(10);

			/*if (rottimer.passsec() > 0.01)
			{
			rottimer.update();
			camer.rotatePosition(-0.05f);
			}*/
			//glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			camer.computematixview();
			camer.computeperspectiveview();
			sdr->Use();
			sdr->set_uniform_int("texturepic", 0);
			sdr->set_uniform_mat_4_4("matixview", (GLfloat*)&camer.m_matixview[0]);
			sdr->set_uniform_mat_4_4("projection_matrix", (GLfloat*)&camer.m_perspectiveview[0]);

			////assimploader.drow(sdr, mat);
			////sdr->Use();
			//boost::timer t;//声明计时器对象并开始计时 
						  // ...测试代码
			render.drow(&assimploader, mat, sdr, "model_matrix", assimploader.get_rootnode());
			//render.drow(&assimploader, mat, sdr,"model_matrix", assimploader.get_rootnode(), 0,ter.elapsed());
			//BOOST_LOG_TRIVIAL(info) << "运行时间：" << t.elapsed() << "s";//输出已流失的时间
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}

	
}