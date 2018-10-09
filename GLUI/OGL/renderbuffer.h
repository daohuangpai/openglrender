#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include "openglerror.h"


namespace GLUI
{
	class renderbuffer {
	public:
		virtual ~renderbuffer() { if (is_available()) glDeleteRenderbuffers(1, &m_renderbufferid); }
		inline bool is_available() { return (glIsRenderbuffer(m_renderbufferid) == GL_TRUE)&& m_generatesuccess; }
		bool Bind() { glBindRenderbuffer(GL_RENDERBUFFER, m_renderbufferid); LAZY_OPENGL_RETURN_FALSE("renderbuffer:Bind: glBindRenderbuffer failed"); return true; }
		//采样数类似使用均值滤波，把图像首先扩大samples​倍然后使用均值缩放的原理得到新图像，使得边缘平滑抗锯齿
		bool generate(GLenum internalformat​, GLsizei width​, GLsizei height, GLsizei samples​ = 0);

#define GET_RENDERBUFFER_PARAM(get_fun,dataptrtype,queuetype)\
        bool get_##get_fun(dataptrtype* oglenum)\
		{\
			if (!Bind()) return false;\
			glGetRenderbufferParameteriv(GL_RENDERBUFFER, queuetype, (GLint*)oglenum);\
			return true;\
		}

		GET_RENDERBUFFER_PARAM(internalformat, GLenum, GL_RENDERBUFFER_INTERNAL_FORMAT)
		GET_RENDERBUFFER_PARAM(sample, GLsizei, GL_RENDERBUFFER_SAMPLES)
		GET_RENDERBUFFER_PARAM(width, GLsizei, GL_RENDERBUFFER_WIDTH)
		GET_RENDERBUFFER_PARAM(height, GLsizei, GL_RENDERBUFFER_HEIGHT)

		GLuint ID() { return m_renderbufferid; }
	private:
		GLuint m_renderbufferid;
		bool m_generatesuccess = false;

		//GL_DEPTH_COMPONENT , GL_DEPTH_COMPONENT16 ,GL_DEPTH_COMPONENT32, or GL_DEPTH_COMPONENT32F .深度格式
		//GL_STENCIL_INDEX , GL_STENCIL_INDEX1 , GL_STENCIL_INDEX4 , GL_STENCIL_INDEX8 , orGL_STENCIL_INDEX16 模板缓冲
		//GL_DEPTH_STENCIL 压缩的深度模板
		//GLenum m_internalformat;//这个缓冲的内部格式，颜色缓冲区就是类似GL_RGBA8这种，或者是深度缓冲
		//GLsizei m_width = 0;
		//GLsizei m_height = 0;
		//GLsizei m_samples​ = 0;//采样数

	};



}