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
#include "../OGL/util.h"
#include "../OGL/shaderhelper.h"
#include "../common/kernels.h"
#include "../OGL/commonbuffer.h"
#include "..\model\dataarray.h"
#include<boost/timer.hpp> 
#include "../OGL/FrameBuffer.h"
#include "../imageproces/convolution.h"
#include "..\imageproces\normalmap.h"

namespace imageprocesstest
{

	const char Gaussianvectex[] = { MAKE_TEXT(
		#version 440 \n
		layout(location = 0) in vec3 position; \n
		layout(location = 1) in vec2 texturecoord; \n
		out vec2 texcoord; \n
		void main(void)\n
	{ \n
		gl_Position = vec4(position,1.0f); \n
		texcoord = texturecoord;\n
	}
	) };

	const char Gaussianfragmenttest[] = {
		MAKE_TEXT(
			#version 440 core\n
			uniform sampler2D process_image; \n
			in vec2 texcoord; \n
			out vec4 color; \n
			void main(void)
			{
				color = vec4(texture2D(process_image, texcoord).xyz, 1.0);
			}
	)
	};

	const char Gaussianfragment[] = {
		MAKE_TEXT(
			#version 440 core\n
			//struct kernelstruct
	  //     {
		 //    float kernel[4];
	  //      };
			layout(std140, binding = 0) buffer kernelbuffer\n
	       {\n
				float kernel[];\n
	        };\n
			//uniform float kernel[25];
			uniform sampler2D process_image; \n
			uniform int kernelwidth; \n
			uniform int kernelheight; \n
			uniform float DeltaY; \n
			uniform float DeltaX; \n
			in vec2 texcoord; \n
			out vec4 color; \n
			vec3 convolution()\n
		    { \n
			vec3 result = vec3(0,0,0);
			int beginx = -(kernelwidth - 1) / 2; \n
	        int beginy = -(kernelheight - 1) / 2; \n
			for (int j = 0; j < kernelheight; j++)\n
			{\n
				for (int i = 0; i < kernelwidth; i++)\n
				{\n
					vec2 tex = vec2(texcoord.x + (beginx + i)*DeltaX, texcoord.y + (beginy + j)*DeltaY); \n
					result = result + texture2D(process_image, tex).xyz*kernel[j*kernelwidth + i]; \n
				}\n
			}\n
			return result;\n
	       }

			void main(void)
			{
				color = vec4(convolution(), 1.0);
			}
		)
	};


	const char computerchar[] = {
		MAKE_TEXT(
			#version 440 core\n
			layout(local_size_x = 1024) in; \n
			//uniform float kernel[225]; \n
			layout(std140, binding = 0) buffer kernelbuffer\n
		    { \n
			float kernel[]; \n
		    }; \n
			uniform int kernelwidth; \n
			uniform int kernelheight; \n
			layout(rgba32f, binding = 0) uniform image2D input_image; \n
			layout(rgba32f, binding = 1) uniform image2D output_image; \n
			void main(void)\n
		   { \n
			ivec2 texcoord = ivec2(gl_GlobalInvocationID.xy); \n
			vec4 result = vec4(0, 0, 0,0); \n
			int beginx = -(kernelwidth - 1) / 2; \n
			int beginy = -(kernelheight - 1) / 2; \n
			ivec2 imsize = imageSize(input_image);
		    for (int j = 0; j < kernelheight; j++)\n
		     { \n
		         for (int i = 0; i < kernelwidth; i++)\n
			      { \n
				   int x = texcoord.x + beginx + i; \n
				   int y = texcoord.y + beginy + j; \n
				   x = min(x, int(imsize.x)); \n
				   x = max(x, int(0)); \n
				   y = min(y, int(imsize.y)); \n
				   y = max(y, int(0)); \n
				   ivec2 bv = ivec2(x,y);\n
				   result = result + imageLoad(input_image, bv.xy)*kernel[j*kernelwidth + i]; \n
		          }\n
		     }\n
		    imageStore(output_image, texcoord.xy, result); \n
	       }
		)
	};

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

		std::shared_ptr<GLUI::image> imageR = GLUI::image::GlobalGet((char*)std::string(PATH::imagepath() + "test.JPG").data());
		std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());
		TextureR->Height = imageR->m_height;
		TextureR->Width = imageR->m_width;
		switch (imageR->m_channle)
		{
		case 1:TextureR->setInternalFormat(GL_LUMINANCE); break;
		case 3:TextureR->setInternalFormat(GL_RGB32F);  break;
		case 4:TextureR->setInternalFormat(GL_RGBA32F); break;
		}

		TextureR->bind_data((char*)imageR->m_data);
		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f,0.0f,1.0f,
			-1.0f,1.0f, 0.0f,0.0f,0.0f,
			1.0f, -1.0f, 0.0f,1.0f,1.0f,
			1.0f, 1.0f, 0.0f,1.0f,0.0f
		};

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });


		 
		//dataarray2D<float> solbelx;
		//GLUI::generate_solbel_kernel_x(3, solbelx);	 
		//std::shared_ptr<GLUI::Texture2D> solbelxtexture = GLUI::convolution(TextureR, solbelx);
		//dataarray2D<float> solbely;
		//GLUI::generate_solbel_kernel_y(3, solbely);
		//std::shared_ptr<GLUI::Texture2D> solbelytexture = GLUI::convolution(TextureR, solbely);
		std::shared_ptr<GLUI::Texture2D> normaltex = makenormalmap(TextureR);
		//std::shared_ptr<GLUI::Texture2D> grayiamge = GLUI::gray(outimagetexture);
		std::shared_ptr<GLUI::shader> sdrshaow(new GLUI::shader);
		sdrshaow->createshader({ { GLUI::shader::VERTEX,(char*)Gaussianvectex,true },
		{ GLUI::shader::FRAGMENT,(char*)Gaussianfragmenttest,true } });
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			sdrshaow->Use();
			normaltex->Active(0);
			normaltex->bind();
			vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}
	int main123123()
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

		std::shared_ptr<GLUI::image> imageR = GLUI::image::GlobalGet((char*)std::string(PATH::imagepath() + "2007_001763.jpg").data());
		std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());
		TextureR->Height = imageR->m_height;
		TextureR->Width = imageR->m_width;
		switch (imageR->m_channle)
		{
		case 1:TextureR->setInternalFormat(GL_LUMINANCE); break;
		case 3:TextureR->setInternalFormat(GL_RGB32F);  break;
		case 4:TextureR->setInternalFormat(GL_RGBA32F); break;
		}

		TextureR->bind_data((char*)imageR->m_data);
		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f,0.0f,1.0f,
			-1.0f,1.0f, 0.0f,0.0f,0.0f,
			1.0f, -1.0f, 0.0f,1.0f,1.0f,
			1.0f, 1.0f, 0.0f,1.0f,0.0f
		};

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });


		int kernelsize = 15;
		float delta = 10.1;
		int totalsize = kernelsize*kernelsize;
		dataarray2D<float> kerneldata;
		kerneldata.init(kernelsize, kernelsize);
		GLUI::generate_gaussian_kernel(delta, kernelsize, kerneldata.get_data());
		std::shared_ptr<GLUI::Texture2D> outimagetexture = GLUI::convolution(TextureR, kerneldata);
		std::shared_ptr<GLUI::Texture2D> grayiamge = GLUI::gray(outimagetexture);
		std::shared_ptr<GLUI::shader> sdrshaow(new GLUI::shader);
		sdrshaow->createshader({ { GLUI::shader::VERTEX,(char*)Gaussianvectex,true },
		{ GLUI::shader::FRAGMENT,(char*)Gaussianfragmenttest,true } });
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			sdrshaow->Use();
			grayiamge->bind();
			vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}

	int main12312()
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

		std::shared_ptr<GLUI::image> imageR = GLUI::image::GlobalGet((char*)std::string(PATH::imagepath() + "2007_001763.jpg").data());
		std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());
		TextureR->Height = imageR->m_height;
		TextureR->Width = imageR->m_width;
		switch (imageR->m_channle)
		{
		case 1:TextureR->setInternalFormat(GL_LUMINANCE); break;
		case 3:TextureR->setInternalFormat(GL_RGB32F);  break;
		case 4:TextureR->setInternalFormat(GL_RGBA32F); break;
		}

		TextureR->bind_data((char*)imageR->m_data);


		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)Gaussianvectex,true },
		{ GLUI::shader::FRAGMENT,(char*)Gaussianfragment,true } });

		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f,0.0f,1.0f,
			-1.0f,1.0f, 0.0f,0.0f,0.0f,
			1.0f, -1.0f, 0.0f,1.0f,1.0f,
			1.0f, 1.0f, 0.0f,1.0f,0.0f
		};

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });

		std::shared_ptr<GLUI::buffer> kernelbuffer;
		int kernelsize = 15;
		float delta = 10.1;
		int totalsize = kernelsize*kernelsize;
		dataarray<float> kerneldata;
		kerneldata.init(totalsize);
		GLUI::generate_gaussian_kernel(delta, kernelsize, kerneldata.get_data());
		SimpleBufferArray<float>::generatebuffer(totalsize * 4,
			GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, kernelbuffer);

		SimpleBufferArray<float> kernel;
		kernel.bindbuffer(kernelbuffer, 0, totalsize * 4);
		kernel.AccessData([&](float* data, int size)->bool
		{
			//return GLUI::generate_gaussian_kernel(delta, kernelsize, data);
			for (int j = 0; j < kernelsize; j++)
			{
				for (int i = 0; i < kernelsize; i++)
				{
					data[j*kernelsize * 4 + i * 4] = kerneldata[j*kernelsize + i];
				}
			}
			//data[7] = 1.0;
			return true;
		}
		);
		/*kernel.AccessData([&](float* data, int size)->bool
		{
		for (int j = 0; j < kernelsize; j++)
		{
		for (int i = 0; i < kernelsize; i++)
		{
		printf("%f,", data[j*kernelsize + i]);
		}
		printf("\r\n");
		}
		return true;
		}
		);*/

		float deltax = 1.0f / imageR->m_width;
		float deltay = 1.0f / imageR->m_height;
		kernelbuffer->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER, 0, 0, kernel.bytesize());

		std::shared_ptr<GLUI::Texture2D> outimagetexture(new GLUI::Texture2D);
		outimagetexture->Height = HEIGHT;
		outimagetexture->Width = WIDTH;
		outimagetexture->setInternalFormat(TextureR->InternalFormat);
		outimagetexture->bind_data(nullptr);
		GLUI::framebuffer framebuffer;
		std::vector<GLUI::framebuffer::attach_struct> attach_param = { { GLUI::framebuffer::DRAW_FRAMEBUFFER,GLUI::framebuffer::COLOR_ATTACHMENT,0,true, outimagetexture.get(),0,0,-1 } };
		std::vector<GLUI::framebuffer::drow_struct> param = { { GLUI::framebuffer::DROW_COLOR_ATTACHMENT,0 } };
		framebuffer.generate(attach_param, param);
		framebuffer.check();
		vao->bind();
		TextureR->bind();
		sdr->Use();
		sdr->set_uniform("kernelwidth", GLUI::glslvalue(kernelsize));
		sdr->set_uniform("kernelheight", GLUI::glslvalue(kernelsize));
		sdr->set_uniform("DeltaY", GLUI::glslvalue(deltay));
		sdr->set_uniform("DeltaX", GLUI::glslvalue(deltax));
		//sdr->set_uniform_Float_array("kernel", kerneldata.get_size(), kerneldata.get_data());
		vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
		opengl_error("hh");
		framebuffer.Unbind();

		std::shared_ptr<GLUI::shader> sdrshaow(new GLUI::shader);
		sdrshaow->createshader({ { GLUI::shader::VERTEX,(char*)Gaussianvectex,true },
		{ GLUI::shader::FRAGMENT,(char*)Gaussianfragmenttest,true } });
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			sdrshaow->Use();
			outimagetexture->bind();
			vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}


	int main123121233()
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

		std::shared_ptr<GLUI::image> imageR = GLUI::image::GlobalGet((char*)std::string(PATH::imagepath() + "container2.png").data());
		std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());
		TextureR->Height = imageR->m_height;
		TextureR->Width = imageR->m_width;
		switch (imageR->m_channle)
		{
		case 1:TextureR->setInternalFormat(GL_LUMINANCE); break;
		case 3:TextureR->setInternalFormat(GL_RGB32F);  break;
		case 4:TextureR->setInternalFormat(GL_RGBA32F); break;
		}

		TextureR->bind_data((char*)imageR->m_data);


 		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)Gaussianvectex,true },
		{ GLUI::shader::FRAGMENT,(char*)Gaussianfragment,true } });
		
		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f,0.0f,1.0f,
			-1.0f,1.0f, 0.0f,0.0f,0.0f,
			1.0f, -1.0f, 0.0f,1.0f,1.0f,
			1.0f, 1.0f, 0.0f,1.0f,0.0f
		};

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });

		std::shared_ptr<GLUI::buffer> kernelbuffer;
		int kernelsize = 15;
		float delta = 10.1;
		int totalsize = kernelsize*kernelsize;
		dataarray<float> kerneldata;
		kerneldata.init(totalsize);
		GLUI::generate_gaussian_kernel(delta, kernelsize, kerneldata.get_data());
		SimpleBufferArray<float>::generatebuffer(totalsize*4,
			GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, kernelbuffer);

		SimpleBufferArray<float> kernel;
		kernel.bindbuffer(kernelbuffer, 0, totalsize*4);
		kernel.AccessData([&](float* data, int size)->bool
		{
			//return GLUI::generate_gaussian_kernel(delta, kernelsize, data);
			for (int j = 0; j < kernelsize; j++)
			{
				for (int i = 0; i < kernelsize; i++)
				{
					data[j*kernelsize * 4 + i * 4] = kerneldata[j*kernelsize+i];
				}
			}
			//data[7] = 1.0;
			return true;
		}
		);
		/*kernel.AccessData([&](float* data, int size)->bool
		{
			for (int j = 0; j < kernelsize; j++)
			{
				for (int i = 0; i < kernelsize; i++)
				{	 
					printf("%f,", data[j*kernelsize + i]);
				}
				printf("\r\n");
			}
			return true;
		}
		);*/

		float deltax = 1.0f / imageR->m_width;
		float deltay= 1.0f / imageR->m_height;
		kernelbuffer->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER, 0, 0, kernel.bytesize());
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			vao->bind();
			TextureR->bind();
			sdr->Use();
			sdr->set_uniform("kernelwidth", GLUI::glslvalue(kernelsize));
			sdr->set_uniform("kernelheight", GLUI::glslvalue(kernelsize));
			sdr->set_uniform("DeltaY", GLUI::glslvalue(deltay));
			sdr->set_uniform("DeltaX", GLUI::glslvalue(deltax));		
			//sdr->set_uniform_Float_array("kernel", kerneldata.get_size(), kerneldata.get_data());
			vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
			opengl_error("hh");
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}


	int main56456()
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

		std::shared_ptr<GLUI::image> imageR = GLUI::image::GlobalGet((char*)std::string(PATH::imagepath() + "container2.png").data());
		std::shared_ptr<GLUI::Texture2D> TextureR(new GLUI::Texture2D());
		TextureR->Height = imageR->m_height;
		TextureR->Width = imageR->m_width;
		switch (imageR->m_channle)
		{
		case 1:TextureR->setInternalFormat(GL_LUMINANCE); break;
		case 3:TextureR->setInternalFormat(GL_RGB32F);  break;
		case 4:TextureR->setInternalFormat(GL_RGBA32F); break;
		}

		TextureR->bind_data((char*)imageR->m_data);
		std::shared_ptr<GLUI::Texture2D> outimagetexture(new GLUI::Texture2D);
		outimagetexture->Height = imageR->m_height;
		outimagetexture->Width = imageR->m_width;
		outimagetexture->setInternalFormat(GL_RGBA32F);
		outimagetexture->bind_data(nullptr);

		std::shared_ptr<GLUI::shader> sdr(new GLUI::shader);
		sdr->createshader({ { GLUI::shader::VERTEX,(char*)Gaussianvectex,true },
		{ GLUI::shader::FRAGMENT,(char*)Gaussianfragmenttest,true } });

		std::shared_ptr<GLUI::shader> computershader(new GLUI::shader);
		computershader->createshader({ { GLUI::shader::COMPUTE,(char*)computerchar,true } });

		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

		GLfloat vertices[] = {
			-1.0f, -1.0f, 0.0f,0.0f,1.0f,
			-1.0f,1.0f, 0.0f,0.0f,0.0f,
			1.0f, -1.0f, 0.0f,1.0f,1.0f,
			1.0f, 1.0f, 0.0f,1.0f,0.0f
		};

		buffer->generate((char*)vertices, sizeof(vertices), GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
		{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } });

		std::shared_ptr<GLUI::buffer> kernelbuffer;
		int kernelsize = 11;
		float delta = 10.1;
		int totalsize = kernelsize*kernelsize;
		dataarray<float> kerneldata;
		kerneldata.init(totalsize);
		GLUI::generate_gaussian_kernel(delta, kernelsize, kerneldata.get_data());
		SimpleBufferArray<float>::generatebuffer(totalsize * 4,
			GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, kernelbuffer);

		SimpleBufferArray<float> kernel;
		kernel.bindbuffer(kernelbuffer, 0, totalsize * 4);
		kernel.AccessData([&](float* data, int size)->bool
		{
			//return GLUI::generate_gaussian_kernel(delta, kernelsize, data);
			for (int j = 0; j < kernelsize; j++)
			{
				for (int i = 0; i < kernelsize; i++)
				{
					data[j*kernelsize * 4 + i * 4] = kerneldata[j*kernelsize + i];
				}
			}
			//data[7] = 1.0;
			return true;
		}
		);

		for (int j = 0; j < kernelsize; j++)
		{
			for (int i = 0; i < kernelsize; i++)
			{
				 printf("%f,", kerneldata[j*kernelsize + i]);
			}
			printf("\r\n");
		}
		float deltax = 1.0f / imageR->m_width;
		float deltay = 1.0f / imageR->m_height;
		boost::timer t;//声明计时器对象并开始计时 
					   //...测试代码
		computershader->Use();
		computershader->set_uniform("kernelwidth", GLUI::glslvalue(kernelsize));
		computershader->set_uniform("kernelheight", GLUI::glslvalue(kernelsize));
		//computershader->set_uniform_Float_array("kernel", kerneldata.get_size(), kerneldata.get_data());
		kernelbuffer->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER, 0, 0, kernel.bytesize());
		//TextureR->bind();
		glBindImageTexture(0, TextureR->ID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		//outimagetexture->bind();
		glBindImageTexture(1, outimagetexture->ID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(outimagetexture->Width, outimagetexture->Height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		BOOST_LOG_TRIVIAL(info) << "运行时间：" << t.elapsed() << "s";//输出已流失的时间
		Sleep(10000);
		while (!glfwWindowShouldClose(window))
		{

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();
			glfwWaitEvents();
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			vao->bind();
			printf("11111111111111111111111111\r\n");
			outimagetexture->bind();
			sdr->Use();
			vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
			opengl_error("hh");
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		return 0;
	}
}