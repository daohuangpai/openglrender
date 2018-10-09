#include "FrameBuffer.h"

namespace GLUI
{

#define ERROROUT(ERROR,STR)\
        case ERROR:\
     {\
	   BOOST_LOG_TRIVIAL(error) << "["<<##ERROR<<"]"<<STR;\
	   break; \
     }\

	MangerTemplateClassCPP(framebufferemanger)

	bool framebuffer::check(TARGET_TYPE target) { 
		Bind(target);
		GLenum  um = glCheckFramebufferStatus(target);
		bool ret = false;
		switch(um)
		{
		case GL_FRAMEBUFFER_COMPLETE:{ret = true;break;}
		ERROROUT(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
			"any of the framebuffer attachment points are framebuffer incomplete.");
		ERROROUT(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
			"the framebuffer does not have at least one image attached to it.");
		ERROROUT(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
			"the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi.");
		ERROROUT(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
			"GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.");
		ERROROUT(GL_FRAMEBUFFER_UNSUPPORTED,
			"the combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");
		ERROROUT(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
			"the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES."
			"or the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.");
		ERROROUT(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,
			"any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.");
		default:break;
		}
		return ret;
	}


	bool framebuffer::attach(attach_struct& attachstruct)
	{
		GLenum oglattachment;
		if (attachstruct.attachment == COLOR_ATTACHMENT)
		{
			//测试是否可以使用
			GLint maxbuffers = 0;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
			if (maxbuffers <= attachstruct.colorattachmentnum){
				BOOST_LOG_TRIVIAL(error) << "framebuffer::attach: unsupport COLOR_ATTACHMENT"<< attachstruct.colorattachmentnum
					                     <<"Max is "<< maxbuffers;
				return false;
			}
			oglattachment = attachstruct.attachment + attachstruct.colorattachmentnum;
		}
		else {
			oglattachment = attachstruct.attachment;
		}

		if (!Bind(attachstruct.target))
			return false;

		if (attachstruct.is_texture)
		{
			switch (attachstruct.texture->TYPE())
			{
			case Texture::TEXTURE_1D:
			{
				glFramebufferTexture1D(attachstruct.target, oglattachment, GL_TEXTURE_1D, ((Texture1D*)attachstruct.texture)->ID(), attachstruct.texturelevel);
				LAZY_OPENGL_RETURN_FALSE("framebuffer::attach: glFramebufferTexture1D failed ")
			}
			break;
			case Texture::TEXTURE_2D:
			{
				glFramebufferTexture2D(attachstruct.target, oglattachment, GL_TEXTURE_2D, ((Texture2D*)attachstruct.texture)->ID(), attachstruct.texturelevel);
				LAZY_OPENGL_RETURN_FALSE("framebuffer::attach: glFramebufferTexture2D failed ")
			}
			break;
			case Texture::TEXTURE_3D:
			{
				glFramebufferTexture3D(attachstruct.target, oglattachment, GL_TEXTURE_3D, ((Texture3D*)attachstruct.texture)->ID(), attachstruct.texturelevel, attachstruct.layer);
				LAZY_OPENGL_RETURN_FALSE("framebuffer::attach: glFramebufferTexture3D failed ")
			}
			break;
			case Texture::TEXTURE_CUBE:
			{
				if (attachstruct.cubeface == 0) {
					glFramebufferTexture(attachstruct.target, oglattachment, ((Texture3D*)attachstruct.texture)->ID(), attachstruct.texturelevel);
					LAZY_OPENGL_RETURN_FALSE("framebuffer::attach: TEXTURE_CUBE glFramebufferTexture failed ")
				}
				else {
					if ((attachstruct.cubeface < cubeTexture::RIGHT) || (attachstruct.cubeface > cubeTexture::BACK)) {
						BOOST_LOG_TRIVIAL(error) << "framebuffer::attach: Texture::TEXTURE_CUBE drow face not available cubeface";
						return false;
					}
					glFramebufferTexture2D(attachstruct.target, oglattachment, attachstruct.cubeface, ((Texture3D*)attachstruct.texture)->ID(), attachstruct.texturelevel);
					LAZY_OPENGL_RETURN_FALSE("framebuffer::attach: glFramebufferTexture2D failed ")
				}

			}
			break;
			default:
				BOOST_LOG_TRIVIAL(error) << "unsupport texture type";
				break;
			}
		}
		else
		{
			glFramebufferRenderbuffer(attachstruct.target, oglattachment, GL_RENDERBUFFER, ((renderbuffer*)attachstruct.render_buffer)->ID());
			LAZY_OPENGL_RETURN_FALSE("framebuffer::attach: glFramebufferRenderbuffer failed ")
		}

		return true;
	}


	bool framebuffer::drowbuffer(std::vector<drow_struct>& param)
	{
		if (!Bind())
			return false;

		GLint maxdrowbuffer = 0;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxdrowbuffer);
		if (param.size() > maxdrowbuffer) {
			BOOST_LOG_TRIVIAL(error) << "framebuffer::drowbuffer: unsupport too much buffer" << param.size()
				<< "Max is " << maxdrowbuffer;
			return false;
		}

		GLint maxattachment = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxattachment);

		std::vector<GLenum> oglparam;
		oglparam.resize(param.size());

		for (int i = 0; i < param.size(); i++)
		{
			drow_struct& drowstruct = param[i];
			if (drowstruct.drowtype == DROW_COLOR_ATTACHMENT)
			{
				if (maxattachment <= drowstruct.colorattachmentnum) {
					BOOST_LOG_TRIVIAL(error) << "framebuffer::drowbuffer: unsupport COLOR_ATTACHMENT" << drowstruct.colorattachmentnum
						<< "Max is " << maxattachment;
					return false;
				}
			   oglparam.push_back(drowstruct.drowtype + drowstruct.colorattachmentnum);
			}
			else
			{
				oglparam.push_back(drowstruct.drowtype);
			}	
		}

		glDrawBuffers(oglparam.size(), &oglparam[0]);
		LAZY_OPENGL_RETURN_FALSE("framebuffer::drowbuffer: glDrawBuffers failed ")
		return true;
	}

	bool framebuffer::generate(std::vector<attach_struct>& attach_param, std::vector<drow_struct>& param)
	{
		if (!is_available()) 
			glGenFramebuffers(1, &m_frameid);

		for (std::vector<attach_struct>::iterator itr = attach_param.begin();
			itr != attach_param.end(); itr++)
		{
			if (!attach(*itr))
				return false;
		}

		return drowbuffer(param);
	}


}