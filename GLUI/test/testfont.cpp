#include "test.h"
#include "../UI/ButtonViews.h"
#include "../UI/glfwindows.h"
#include "../UI/Glwindows.h"
#include "../tool/path.h"
#include "../OGL/shader.h"
#include "../OGL/font.h"
#include "../tool/path.h"
#include "../OGL/buffer.h"
#include "../OGL/Vao.h"

namespace fonttest
{
#define WIDTH   640
#define HEIGHT  480

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
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "testfont", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glewInit();
		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());



		TextureR->Mipmapping = true;
		TextureR->Width = width;
		TextureR->Height = height;
		TextureR->bind_data(NULL);
		GLUI::Font font;
		font.init("c:/windows/fonts/simkai.ttf");
		//font.set_italic(true);
		font.set_font_size(32, 32);
		font.DrowText(TextureR, 0, 0, L"      ≤‚ ‘\nSTLport is a free, fast and secure STL replacement that works with all major compilers and platforms. To get it, download the latest release from <stlport.org>. Usually you'll just need to run 'configure' + a makefile (see their README for more details). Don't miss to add <stlport_root>/stlport to your compiler's default include paths - prior to the directory where your compiler vendor's headers lie. Do the same for <stlport_root>/lib and recompile assimp. To ensure you're really building against STLport see aiGetCompileFlags(). In our testing, STLport builds tend to be a bit faster than builds against Microsoft's C++ Standard Library.", 0);
		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)std::string(PATH::shaderpath() + "base_object_vectex_shader.vs").data(),false },
						  { GLUI::shader::FRAGMENT,(char*)std::string(PATH::shaderpath() + "base_object_fragment_shader.gs").data(),false } });
		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f,0.0f,1.0f,
			-1.0f,1.0f, 0.0f,0.0f,0.0f,
			1.0f, -1.0f, 0.0f,1.0f,1.0f,
			1.0f, 1.0f, 0.0f,1.0f,0.0f
		};

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer( { { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });

		TextureR->Active(0);
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			sdr->Use();
			vao->bind();
			TextureR->bind();
			vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
			opengl_error("hh");
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}
}