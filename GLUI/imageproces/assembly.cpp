#include "assembly.h"

namespace GLUI
{

	std::shared_ptr<GLUI::Vao> get_image_process_vao()
	{
		std::shared_ptr<GLUI::Vao> vao;
		if (!Vaomanger::shareVaomanger()->get(std::string("GLUI_IMAGE_PROCESS"), vao))
		{
			std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());

			GLfloat vertices[] = {
				-1.0f, -1.0f, 0.0f,0.0f,0.0f,
				-1.0f,1.0f, 0.0f,0.0f,1.0f,
				1.0f, -1.0f, 0.0f,1.0f,0.0f,
				1.0f, 1.0f, 0.0f,1.0f,1.0f
			};

			if (!buffer->generate((char*)vertices, sizeof(vertices),
				GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER))
			{
				return nullptr;
			}

			vao.reset(new GLUI::Vao);
			if (!vao->BindBuffer({ { buffer, 5 * sizeof(GL_FLOAT),0,3,GL_FLOAT,GL_FALSE,0 },
			{ buffer, 5 * sizeof(GL_FLOAT), 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } }))
			{
				return nullptr;
			}
			Vaomanger::shareVaomanger()->set(std::string("GLUI_IMAGE_PROCESS"), vao);
		}
		return vao;
	}

	std::shared_ptr<GLUI::framebuffer> get_image_process_framebuffer(std::shared_ptr<GLUI::Texture2D>& tex)
	{
		std::shared_ptr<GLUI::framebuffer> framebuffer;
		std::vector<GLUI::framebuffer::attach_struct> attach_param = { { GLUI::framebuffer::DRAW_FRAMEBUFFER,GLUI::framebuffer::COLOR_ATTACHMENT,0,true, tex.get(),0,0,-1 } };
		std::vector<GLUI::framebuffer::drow_struct> param = { { GLUI::framebuffer::DROW_COLOR_ATTACHMENT,0 } };
		framebuffer.reset(new GLUI::framebuffer);
		if (!framebuffer->generate(attach_param, param) || !framebuffer->check()) {
				return nullptr;
		}
		return framebuffer;
	}


	std::shared_ptr<GLUI::Texture2D> makeemptytex2D(std::shared_ptr<GLUI::Texture2D>& tex)
	{
		std::shared_ptr<Texture2D> outimagetexture(new Texture2D);
		outimagetexture->Height = tex->Height;
		outimagetexture->Width = tex->Width;
		outimagetexture->setInternalFormat(tex->InternalFormat);
		if (!outimagetexture->bind_data(nullptr))
			return nullptr;
		return outimagetexture;
	}
}