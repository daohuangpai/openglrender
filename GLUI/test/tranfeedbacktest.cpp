#include "test.h"

#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "../OGL/camera.h"
#include "../OGL/vao.h"
#include <boost\signals2\signal.hpp>
#include "../OGL/feedback.h"
#include "../OGL/shader.h"

namespace tranfeedback
{
	int main()
	{
		const GLchar* vertexShaderSrc =
			"#version 440 core\n"
			"layout (location = 0) in vec3 pos;\n"
			"layout (location = 1) in vec3 vel;\n"
			"layout (location = 2) in vec3 col;\n"
			"out vec3 outValue;\n"

			"void main()\n"
			"{\n"
			"outValue = sqrt(pos);\n"
			"}\n";
		glfwInit();
		// Set all the required options for GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_RESIZABLE, true);

		// Create a GLFWwindow object that we can use for GLFW's functions
		GLFWwindow* window = glfwCreateWindow(1024, 1024, "LearnOpenGL", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glewInit();
		GLUI::transformfeedback feedbackshader;
		std::shared_ptr<GLUI::shader> m_feedbackshader;
		m_feedbackshader.reset(new GLUI::shader);
		m_feedbackshader->createshader({ { GLUI::shader::VERTEX,(char*)vertexShaderSrc,true }},
			[&](GLuint program)->bool
		{
			char* outlist[] = { "outValue" };
			return feedbackshader.init(program, GLUI::transformfeedback::INTERLEAVED_ATTRIBS, outlist, 1);
		}
		);
		
		bool ret = m_feedbackshader->is_available();

		GLUI::Vao vao;
		int numParticles = 10;
		std::vector<glm::vec3> data;
		std::vector<glm::vec3> feedback;

		for (int i = 0; i < numParticles; i++) {
			glm::vec3 pos = glm::vec3(16.0f, 16.0f, 16.0f);
			glm::vec3 vel = glm::vec3(16.0f, 16.0f, 16.0f);
			glm::vec3 col = glm::vec3(16.0f, 16.0f, 16.0f);
			glm::vec3 emptyVec = glm::vec3(0.0f, 0.0f, 0.0f);
			data.push_back(pos);
			data.push_back(vel);
			data.push_back(col);

			feedback.push_back(emptyVec);
		}
		glGetError();
		std::shared_ptr<GLUI::buffer> buffer;
		buffer.reset(new GLUI::buffer());
		buffer->generate((char*)&data[0], data.size() * sizeof(glm::vec3), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		vao.BindBuffer({ { buffer, 9 * sizeof(GLfloat),  0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 9 * sizeof(GLfloat),  1,3,GL_FLOAT,GL_FALSE,(3 * sizeof(GLfloat)) },
		{ buffer, 9 * sizeof(GLfloat),  2,3,GL_FLOAT,GL_FALSE,(6 * sizeof(GLfloat)) }
		});

		std::shared_ptr<GLUI::buffer> tbo;
		tbo.reset(new GLUI::buffer());
		tbo->generate((char*)&data[0], data.size() * sizeof(glm::vec3), GLUI::buffer::STATIC_READ, GLUI::buffer::ARRAY_BUFFER);
		feedbackshader.BindOutPutBuffer(tbo, 0);
		int i = 0;
		while (i++ < 10)
		{
			std::cout << "i : " << i << std::endl;
			m_feedbackshader->Use();
			// Perform transform feedback
			glEnable(GL_RASTERIZER_DISCARD);
			feedbackshader.Drow(&vao, GLUI::Vao::POINTS, 0, numParticles);
			glDisable(GL_RASTERIZER_DISCARD);
			glFlush();

			// GLfloat feedback[5];
			tbo->CopyOutDataNotHuge((char*)&feedback[0], 0, feedback.size() * sizeof(glm::vec3));
			for (int i = 0; i < numParticles; i++) {
				data[3 * i] = feedback[i];
				debug_common_info(data[3 * i][0]);
				debug_common_info(data[3 * i][1]);
				debug_common_info(data[3 * i][2]);
			}
			buffer->UpdateDataHuge((char*)&data[0], 0, data.size() * sizeof(glm::vec3));
		}
		getchar();
		return 0;
	}
}