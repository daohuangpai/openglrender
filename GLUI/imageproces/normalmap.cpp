#include "normalmap.h"
#include "../common/kernels.h"

namespace GLUI
{

	const char normalvectex[] = { MAKE_TEXT(
		#version 440 \n
		layout(location = 0) in vec3 position; \n
		layout(location = 1) in vec2 texturecoord; \n
		out vec2 texcoord; \n
		void main(void)\n
	   { \n
		gl_Position = vec4(position,1.0f); \n
		texcoord = texturecoord; \n
	   }
	   ) };


	const char normalfragment[] = {
		MAKE_TEXT(
			#version 440 core\n
			uniform sampler2D solbelximage; \n		
			uniform sampler2D solbelyimage;\n
			in vec2 texcoord; \n
			out vec4 color; \n
			void main(void)
			{
		    float zx = texture2D(solbelximage, texcoord).x;
			float zy = texture2D(solbelyimage, texcoord).x;
			color = vec4(normalize(vec3((-zx+1.0f)/2.0f, (-zy + 1.0f) / 2.0f, 2.0f)),1.0);
			}
	    )
	};



	std::shared_ptr<Texture2D> makenormalmap(IN std::shared_ptr<Texture2D>& src_tex)
	{
		dataarray2D<float> gsdata;
		GLUI::generate_gaussian_kernel(2.0,3, gsdata);
		std::shared_ptr<Texture2D> gstex = GLUI::convolution(src_tex, gsdata);
		if (!gstex)
			return nullptr;
		std::shared_ptr<Texture2D> tex = GLUI::gray(gstex);
		if (!tex)
			return nullptr;

		dataarray2D<float> solbelx;
		GLUI::generate_solbel_kernel_x(solbelx);
		std::shared_ptr<GLUI::Texture2D> solbelxtexture = GLUI::convolution(tex, solbelx);
		if (!solbelxtexture.get())
			return nullptr;

		dataarray2D<float> solbely;
		GLUI::generate_solbel_kernel_y(solbely);
		std::shared_ptr<GLUI::Texture2D> solbelytexture = GLUI::convolution(tex, solbely);
		if (!solbelytexture.get())
			return nullptr;

		std::shared_ptr<GLUI::shader> sdr;
		if (!shadermanger::shareshadermanger()->get(std::string("GLUI_NORMALMAP"), sdr))
		{
			sdr.reset(new GLUI::shader);
			if (!sdr->createshader({ { GLUI::shader::VERTEX,(char*)normalvectex,true },
			{ GLUI::shader::FRAGMENT,(char*)normalfragment,true } })) {
				return nullptr;
			}
			shadermanger::shareshadermanger()->set(std::string("GLUI_NORMALMAP"), sdr);
		}

		std::shared_ptr<Texture2D> outimagetexture = makeemptytex2D(src_tex);
		if (!outimagetexture.get())
			return nullptr;
		std::shared_ptr<GLUI::framebuffer> framebuffer = get_image_process_framebuffer(outimagetexture);
		if (!framebuffer.get())
			return nullptr;

		std::shared_ptr<GLUI::Vao> vao = get_image_process_vao();
		if (!vao.get())
			return nullptr;
		solbelytexture->bind();
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, src_tex->Width, src_tex->Height);
		framebuffer->Bind();
		sdr->Use();
		sdr->set_uniform_int("solbelximage", 0);
		sdr->set_uniform_int("solbelyimage", 1);
		solbelxtexture->Active(0);
		solbelxtexture->bind();
		solbelytexture->Active(1);
		solbelytexture->bind();
		vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
		framebuffer->Unbind();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		return outimagetexture;
	}






}