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
#include "..\computer\BNT.h"


namespace Shadowtest
{
	bool usepra = true;
	float speed = 0.3f;
	float heightscale = 0.1f;
	GLUI::camera camer;
	// Function prototypes
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (action != GLFW_PRESS) return;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
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
		else if (key == GLFW_KEY_G)
		{
			usepra = !usepra;
		}
		else if (key == GLFW_KEY_T)
		{
			heightscale += 0.05f;
		}
		else if (key == GLFW_KEY_B)
		{
			heightscale -= 0.05f;
		}
	}


	float vectexdata[] =
	{
		1.000000,1.000000,-1.000000,
		1.000000,-1.000000,-1.000000,
		-1.000000,-1.000000,-1.000000,
		1.000000,1.000000,-1.000000,
		-1.000000,-1.000000,-1.000000,
		-1.000000,1.000000,-1.000000,
		1.000000,0.999999,1.000000,
		-1.000000,1.000000,1.000000,
		-1.000000,-1.000000,1.000000,
		1.000000,0.999999,1.000000,
		-1.000000,-1.000000,1.000000,
		0.999999,-1.000001,1.000000,
		1.000000,1.000000,-1.000000,
		1.000000,0.999999,1.000000,
		0.999999,-1.000001,1.000000,
		1.000000,1.000000,-1.000000,
		0.999999,-1.000001,1.000000,
		1.000000,-1.000000,-1.000000,
		1.000000,-1.000000,-1.000000,
		0.999999,-1.000001,1.000000,
		-1.000000,-1.000000,1.000000,
		1.000000,-1.000000,-1.000000,
		-1.000000,-1.000000,1.000000,
		-1.000000,-1.000000,-1.000000,
		-1.000000,-1.000000,-1.000000,
		-1.000000,-1.000000,1.000000,
		-1.000000,1.000000,1.000000,
		-1.000000,-1.000000,-1.000000,
		-1.000000,1.000000,1.000000,
		-1.000000,1.000000,-1.000000,
		1.000000,0.999999,1.000000,
		1.000000,1.000000,-1.000000,
		-1.000000,1.000000,-1.000000,
		1.000000,0.999999,1.000000,
		-1.000000,1.000000,-1.000000,
		-1.000000,1.000000,1.000000
	};

	float texturedata[] =
	{
		0.000000,0.000000,
		1.000000,0.000000,
		1.000000,1.000000,
		0.000000,0.000000,
		1.000000,1.000000,
		0.000000,1.000000,
		0.000000,0.000000,
		1.000000,0.000000,
		1.000000,1.000000,
		0.000000,0.000000,
		1.000000,1.000000,
		0.000000,1.000000,
		0.000000,0.000000,
		1.000000,0.000000,
		1.000000,1.000000,
		0.000000,0.000000,
		1.000000,1.000000,
		0.000000,1.000000,
		0.000000,0.000000,
		1.000000,0.000000,
		1.000000,1.000000,
		0.000000,0.000000,
		1.000000,1.000000,
		0.000000,1.000000,
		0.000000,0.000000,
		1.000000,0.000000,
		1.000000,1.000000,
		0.000000,0.000000,
		1.000000,1.000000,
		0.000000,1.000000,
		0.000000,0.000000,
		1.000000,0.000000,
		1.000000,1.000000,
		0.000000,0.000000,
		1.000000,1.000000,
		0.000000,1.000000
	};


	int main()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_RESIZABLE, true);


		// Create a GLFWwindow object that we can use for GLFW's functions
		GLFWwindow* window = glfwCreateWindow(1024, 1024, "testassimp", nullptr, nullptr);
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
		glewInit();
		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		bool nu = sdr->createshader({ { GLUI::shader::VERTEX,(char*)std::string(PATH::shaderpath() + "material_test_vectex_shader.vs").data(),false },
		{ GLUI::shader::FRAGMENT,(char*)std::string(PATH::shaderpath() + "material_test_fragment_shader.fs").data(),false }
		});
		//glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		bool use = true;
		bool useline = true;
		glftimer rottimer;
		camer.zNear = 1;
		camer.zFar = -3;
		camer.m_position = glm::vec3(0.9f, 0.0f, -1.6f);
		camer.m_lookat_position = glm::vec3(0.0f, 0.0f, 0.0f);
		float v[] = { 1.0f,0.0f,0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		/*	glm::mat4 mat = glm::make_mat4(v);
		float scale = 10.0f / max(max(size.x, size.y), size.z);
		glm::vec3 middele = aabb.min + size / 2.0f;
		mat = glm::translate(mat, -middele);
		mat = glm::translate(mat, glm::vec3(-5.0f, 0.0f, 0.0f));
		mat = glm::scale(glm::vec3(scale, -scale, scale));*/
		dataarray<glm::vec2> uv;
		dataarray<glm::vec3> vectex;
		dataarray<unsigned int> element;
		dataarray<glm::vec3> normal;
		dataarray<glm::vec3> tangent;
		dataarray<glm::vec3> biangent;
		GLUI::Vao::DROW_TYPE drowtype = GLUI::Vao::TRIANGLES;
		vectex.init(sizeof(vectexdata) / sizeof(glm::vec3));
		memcpy(vectex.get_data(), vectexdata, vectex.get_byte_size());
		uv.init(sizeof(texturedata) / sizeof(glm::vec2));
		memcpy(uv.get_data(), texturedata, uv.get_byte_size());
		COMPUTER::BNT_PARAM bnt_param;
		bnt_param.vectex = &vectex;
		bnt_param.uv = &uv;
		bnt_param.drowtype = drowtype;
		bnt_param.normal = &normal;
		bnt_param.tangent = &tangent;
		bnt_param.biangent = &biangent;
		bool ret = COMPUTER::computerBNTVector(bnt_param);


		std::shared_ptr<GLUI::Texture2D> diffuseTexture;
		diffuseTexture.reset(GLUI::Texture2D::CreateTexture2D((char*)std::string(PATH::imagepath() + "container/container2_COLOR.png").data()));
		std::shared_ptr<GLUI::Texture2D> normalmapTexture;
		normalmapTexture.reset(GLUI::Texture2D::CreateTexture2D((char*)std::string(PATH::imagepath() + "container/container2_NRM.png").data()));
		std::shared_ptr<GLUI::Texture2D> specularTexture;
		specularTexture.reset(GLUI::Texture2D::CreateTexture2D((char*)std::string(PATH::imagepath() + "container/container2_SPEC.png").data()));
		std::shared_ptr<GLUI::Texture2D> parallaxTexture;
		parallaxTexture.reset(GLUI::Texture2D::CreateTexture2D((char*)std::string(PATH::imagepath() + "container/container2_DISP.png").data()));

		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		camer.zNear = 1;
		camer.zFar = -3;
		camer.m_position = glm::vec3(0.9f, 0.0f, -1.6f);

		std::shared_ptr<GLUI::buffer> vectexbuffer(new GLUI::buffer());
		vectexbuffer->generate((char*)vectexdata, sizeof(vectexdata), buffer::STATIC_DRAW, buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::buffer> texturebuffer(new GLUI::buffer());
		texturebuffer->generate((char*)texturedata, sizeof(texturedata), buffer::STATIC_DRAW, buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::buffer> normalbuffer(new GLUI::buffer());
		normalbuffer->generate((char*)normal.get_data(), normal.get_byte_size(), buffer::STATIC_DRAW, buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::buffer> biangentbuffer(new GLUI::buffer());
		biangentbuffer->generate((char*)biangent.get_data(), biangent.get_byte_size(), buffer::STATIC_DRAW, buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::buffer> tangentbuffer(new GLUI::buffer());
		tangentbuffer->generate((char*)tangent.get_data(), tangent.get_byte_size(), buffer::STATIC_DRAW, buffer::ARRAY_BUFFER);


		//normalbuffer->AccessData([](char* data, int size)
		//{
		//	glm::vec3* ptr = (glm::vec3*)data;
		//	int length = size / sizeof(glm::vec3);
		//	for (int i = 0; i < length; i++)
		//	{
		//		printf("(%f,%f,%f)\r\n", ptr[i][0], ptr[i][1], ptr[i][2]);
		//	}
		//});
		vao->BindBuffer({ { vectexbuffer, 0,0,3,GL_FLOAT,GL_FALSE,0 },
		{ normalbuffer, 0,1,3,GL_FLOAT,GL_FALSE,0 },
		{ texturebuffer, 0,2,2,GL_FLOAT,GL_FALSE,0 } ,
		{ biangentbuffer, 0,3,3,GL_FLOAT,GL_FALSE,0 },
		{ tangentbuffer, 0,4,3,GL_FLOAT,GL_FALSE,0 } });

		GLUI::lightmodel model[3];

		model[0].enable = 1;
		model[0].directionlight = 0;
		model[0].spotlight = 0;
		model[0].m_spot_direction = glm::vec3(1.0f, 0.0f, 1.0f);
		model[0].m_position = glm::vec3(1.0f, 0.0f, 4.0f);
		model[0].m_ambientlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//环境光，前三个颜色
		model[0].m_diffuselightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//漫反射，前三个颜色
		model[0].m_specularlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//反射光，前三个颜色


		model[1].enable = 1;
		model[1].directionlight = 1;
		model[1].spotlight = 1;
		model[1].m_spot_direction = glm::vec3(-5.0f, 0.0f, -4.0f);
		model[1].m_spot_cutoff = 20;
		model[1].m_position = glm::vec3(5.0f, 0.0f, 4.0f);
		model[1].m_ambientlightcolor = glm::vec3(1.0f, 0.0f, 0.0f);//环境光，前三个颜色
		model[1].m_diffuselightcolor = glm::vec3(1.0f, 0.0f, 0.0f);//漫反射，前三个颜色
		model[1].m_specularlightcolor = glm::vec3(1.0f, 0.0f, 0.0f);//反射光，前三个颜色



		model[2].enable = 1;
		model[2].directionlight = 1;
		model[2].spotlight = 1;
		model[2].m_spot_direction = glm::vec3(-5.0f, 0.0f, 4.0f);
		model[2].m_spot_cutoff = 10;
		model[2].m_position = glm::vec3(5.0f, 0.0f, -4.0f);
		model[2].m_ambientlightcolor = glm::vec3(0.0f, 1.0f, 0.0f);//环境光，前三个颜色
		model[2].m_diffuselightcolor = glm::vec3(0.0f, 1.0f, 0.0f);//漫反射，前三个颜色
		model[2].m_specularlightcolor = glm::vec3(0.0f, 1.0f, 0.0f);//反射光，前三个颜色


		std::map<std::string, std::string> funnames;
		funnames["ComputerTBNModel"] = "TBNnormalization";
		sdr->Use();
		GLUI::lightbuffer lightbuffer;
		lightbuffer.init(model, sizeof(model) / sizeof(GLUI::lightmodel));
		lightbuffer.Bind(sdr.get());
		sdr->set_subroutine(GLUI::shader::FRAGMENT, funnames);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		//glFrontFace(GL_CW);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			Sleep(10);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			camer.computematixview();
			camer.computeperspectiveview();
			sdr->Use();
			sdr->set_uniform_mat_4_4("matixview", (GLfloat*)&camer.m_matixview[0]);
			sdr->set_uniform_mat_4_4("projection_matrix", (GLfloat*)&camer.m_perspectiveview[0]);
			sdr->set_uniform_mat_4_4("model_matrix", (GLfloat*)&glm::make_mat4(v)[0]);
			sdr->set_uniform_float_vec3("eyePos", camer.m_position[0], camer.m_position[1], camer.m_position[2]);
			sdr->set_uniform_float("heightscale", heightscale);
			sdr->set_uniform_float("useparallaxmap", usepra);
			vao->bind();
			opengl_error("hh");
			sdr->set_uniform_int("diffusetex", 0);
			diffuseTexture->bind(0);
			sdr->set_uniform_int("normalmaptex", 1);
			normalmapTexture->bind(1);
			sdr->set_uniform_int("speculartex", 2);
			specularTexture->bind(2);
			sdr->set_uniform_int("parallaxtex", 3);
			parallaxTexture->bind(3);
			opengl_error("hh");
			vao->DrowArray(GLUI::Vao::TRIANGLES, 0, vectexbuffer->getsize() / sizeof(glm::vec3));
			opengl_error("hh");
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}
}