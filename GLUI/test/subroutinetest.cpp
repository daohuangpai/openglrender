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

namespace subroutinetest
{

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,1.0f,1.0f,
		0.5f, -0.5f, 0.0f,1.0f,1.0f,
		0.5f, 0.5f, 0.0f,1.0f,1.0f,
		-0.5f, 0.5f, 0.0f,1.0f,1.0f
	};

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
		GLFWwindow* window = glfwCreateWindow(1024, 1024, "subroutinetest", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glewInit();
		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字  
		const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台  
		const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号  

		printf("OpenGL实现厂商的名字：%s\n", name);
		printf("渲染器标识符：%s\n", biaoshifu);
		printf("OpenGL实现的版本号：%s\n", OpenGLVersion);

		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		std::shared_ptr<GLUI::buffer> Atomicbuffer(new GLUI::buffer());
		Atomicbuffer->generate(NULL, sizeof(GLuint), GLUI::buffer::DYNAMIC_DRAW, GLUI::buffer::ATOMIC_COUNTER_BUFFER);
		GLuint Atomicnum = 0;
		Atomicbuffer->UpdateDataNotHuge((char*)&Atomicnum, 0, sizeof(GLuint));
		Atomicbuffer->BindToBinding(Atomicbuffer->get_target(), 0);
		vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT), 0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });
		/*vao->BindBuffer(Atomicbuffer, sizeof(GLuint), { {2,1,GL_UNSIGNED_INT,GL_FALSE,0 }});*/


		GLUI::camera camer;
		camer.zNear = 0;
		camer.zFar = 3;

		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)std::string(PATH::shaderpath() + "subroutine_test_vectex_shader.vs").data(),false },
		{ GLUI::shader::FRAGMENT,(char*)std::string(PATH::shaderpath() + "subroutine_test_fragment_shader.fs").data(),false } });
		sdr->Use();
		std::string strname;
		sdr->get_active_subroutine_uniform_name(GLUI::shader::FRAGMENT, sdr->get_subroutine_uniform(GLUI::shader::FRAGMENT, "veccolorModel"), strname);
		BOOST_LOG_TRIVIAL(info) << strname;

		BOOST_LOG_TRIVIAL(info) << "vecRcolor:" <<
			sdr->get_subroutine_index(GLUI::shader::FRAGMENT, "vecRcolor") << "\r\n" <<
			"vecGcolor:" <<
			sdr->get_subroutine_index(GLUI::shader::FRAGMENT, "vecGcolor") << "\r\n" <<
			"vecBcolor:" <<
			sdr->get_subroutine_index(GLUI::shader::FRAGMENT, "vecBcolor") << "\r\n" <<
			"veccolorRscale:" <<
			sdr->get_subroutine_index(GLUI::shader::FRAGMENT, "veccolorRscale") << "\r\n" <<
			"veccolorGscale:" <<
			sdr->get_subroutine_index(GLUI::shader::FRAGMENT, "veccolorGscale") << "\r\n" <<
			"veccolorBcale:" <<
			sdr->get_subroutine_index(GLUI::shader::FRAGMENT, "veccolorBcale") << "\r\n";

		char* veccolorModelfuns[] = { "vecRcolor","vecGcolor","vecBcolor" };
		char* veccolorscaleModelfuns[] = { "veccolorRscale","veccolorGscale","veccolorBcale" };
		std::map<std::string, std::string> funnames = { { "veccolorModel", "vecRcolor" },
		{ "veccolorscaleModel", "veccolorRscale" } };
		funnames["ComputerNormalMapModel"] = "ComputerNormalMapSchmidt";
		double veccolorModeltime = glfwGetTime();
		double veccolorscaletime = glfwGetTime();
		int veccolorModelcount = 0;
		int veccolorscaleModelcount = 0;
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			Sleep(30);
			camer.computematixview();
			camer.computeperspectiveview();
			sdr->Use();
			double buffertime = glfwGetTime();
			if (buffertime - veccolorModeltime > 0.9) {
				funnames["veccolorModel"] = veccolorModelfuns[veccolorModelcount++ % 3];
				veccolorModeltime = buffertime;
			}
			if (buffertime - veccolorscaletime > 0.3) {
				funnames["veccolorscaleModel"] = veccolorscaleModelfuns[veccolorscaleModelcount++ % 3];
				veccolorscaletime = buffertime;
			}
			sdr->set_subroutine(GLUI::shader::FRAGMENT, funnames);
			vao->bind();
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(102.0f);
			vao->DrowArray(GLUI::Vao::POINTS, 0, 4);//GL_POINT是错误的，这个太隐藏错误了
			opengl_error("hh");
			glfwSwapBuffers(window);

		}
		glfwTerminate();
		return 0;


	}

}

