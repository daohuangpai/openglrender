#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include "openglerror.h"
#include "texture.h"
#include "renderbuffer.h"
#include <vector>
#include "mangertemplate.h"

namespace GLUI
{
	/*
	初始化
	generate attach drowbuffer
	*/

	//就是将不同的texture绑定到渲染管线，从而直接获得输出
	class framebuffer {
	public:
		enum TARGET_TYPE
		{
			//glBlitFramebuffer帧缓冲区位块传送可以高效的将一个矩形区域的像素值从一个帧缓冲区（读帧缓冲区）复制到另一个帧缓冲区（绘图帧缓冲区）。帧缓冲区位块传送的关键应用之一是将一个多重采样渲染缓冲区解析为一个纹理（用一个帧缓冲区对象，纹理绑定为它的颜色附着）。
			//即bind的不同纹理间的拷贝，绑定为READ_FRAMEBUFFER到DRAW_FRAMEBUFFER
			DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,//作为渲染的缓冲区，相当于写
			READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,//作为读取缓冲区数据的缓存
			FRAMEBUFFER = GL_FRAMEBUFFER//作为上面两个的结合体
		};

		//GL_MAX_COLOR_ATTACHMENTS - 1
		enum ATTACHMENT
		{
			//颜色缓冲区数量从GL_COLOR_ATTACHMENT0到GL_COLOR_ATTACHMENT0 + (maxbuffers-1)(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers))
			//这个起始是和glsl中fragment的输出有关，输出有两个
			/*
			 比如layout (location = 0) out vec4 FragColor;
             layout (location = 1) out vec4 BrightColor;
			 那么分别绑定为GL_COLOR_ATTACHMENT0和GL_COLOR_ATTACHMENT1，而这两个也分别绑定为两个不同的texture，那么那两个输出将输出到这个buffer
			 https://blog.csdn.net/jxw167/article/details/60764506
			*/
			COLOR_ATTACHMENT = GL_COLOR_ATTACHMENT0,
			DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT,
			STENCIL_ATTACHMEN= GL_STENCIL_ATTACHMENT,
			DEPTH_STENCIL_ATTACHMENT=GL_DEPTH_STENCIL_ATTACHMENT
		};

		//使用双缓冲绘图的话
		//FRONT---表示当前显示的图像
		//BACK---表示要交换的缓存图像
		//LEFT表示法线的方向，和纹理相关的,顺时针方向法线
		//RIGHT 表示法线的方向，和纹理相关的,逆时针方向法线

		enum DROW_TYPE
		{
			NONE = GL_NONE,//The fragment shader output value is not written into any color buffer.	
			FRONT_LEFT = GL_FRONT_LEFT,//The fragment shader output value is written into the front left color buffer.
			FRONT_RIGHT = GL_FRONT_RIGHT,//The fragment shader output value is written into the front right color buffer.	
			BACK_LEFT = GL_BACK_LEFT, //The fragment shader output value is written into the back left color buffer.	
			BACK_RIGHT = GL_BACK_RIGHT,//The fragment shader output value is written into the back right color buffer.	
			DROW_COLOR_ATTACHMENT = GL_COLOR_ATTACHMENT0//The fragment shader output value is written into the nth color attachment of the current framebuffer. n may range from 0 to the value of GL_MAX_COLOR_ATTACHMENTS.
		};

		virtual ~framebuffer() { glDeleteFramebuffers(1, &m_frameid); }
		
		bool Bind(TARGET_TYPE target) { m_target = target; glBindFramebuffer(m_target, m_frameid); LAZY_OPENGL_RETURN_FALSE("framebuffer:Bind: glBindFramebuffer failed"); return true; }
		bool Bind() { glBindFramebuffer(m_target, m_frameid); LAZY_OPENGL_RETURN_FALSE("framebuffer:Bind: glBindFramebuffer failed"); return true;}
		inline bool is_available() { return (glIsFramebuffer(m_frameid) == GL_TRUE)&& check(); }
		bool Unbind() { glBindFramebuffer(m_target, 0); LAZY_OPENGL_RETURN_FALSE("framebuffer:Unbind: glBindFramebuffer failed"); return true; }

		//FBO里每个附着点都对能附着的Image的格式有要求。但是，如果附着了不符合要求的Image，不会立即产生GL error。在使用不合适的设置的FBO时才会引发错误。为了安全地使用FBO，必须检测各种可能出现的问题（例如Image的大小等）。
		//一个可以正常使用的FBO被称作是“完整的FBO”，检测完整度
		bool check(TARGET_TYPE target);
		bool check() { return check(m_target); }
		
		//纹理绑定渲染的哪个部分，如果是绑定颜色缓冲那么该值表示第几个颜色缓冲，也就是glsl可以输出多个(ATTACHMENT中有解释)，，
		struct attach_struct
		{
			TARGET_TYPE target;
			//common
			ATTACHMENT attachment;//下面纹理绑定的颜色缓冲区的类型
			unsigned int colorattachmentnum;//如果是COLOR_ATTACHMENT,glsl的out属性的输出，那么才有意义表明是第几个
			//must set
			bool is_texture;//判断下面的联合体是哪个
			union {
				Texture*  texture;//绑定的纹理
				renderbuffer* render_buffer;//绑定的渲染缓冲，和纹理只能有一个有
			};

			//for texture only
			GLint texturelevel;//绑定的第几级mip
			//for 3Dtexture only
			GLint layer;//仅仅3Dtexture有意义
			//for cube texture only
			int cubeface;//如果是cube texture，这个就是要绑定的纹理的面,如果是-1，则绑定全部，否则绑定就是cubeTexture::CUBE_TEXTURET_FACE
		};
		//本身buffer是没有任何缓冲区的，该函数附加缓冲区，随时可以切换
		bool attach(attach_struct& attachstruct);//仅当attachment == COLOR_ATTACHMENT时colorattachmentnum有意义

		struct drow_struct
		{
			DROW_TYPE drowtype;
			unsigned int colorattachmentnum;//如果是COLOR_ATTACHMENT，那么才有意义表明是第几个
		};
		bool drowbuffer(std::vector<drow_struct>& param);


		bool generate(std::vector<attach_struct>& attach_param, std::vector<drow_struct>& param);

	private:
		GLuint m_frameid;
		TARGET_TYPE m_target;

	};
	MangerTemplateClassPtr(std::string, framebuffer, framebufferemanger)
}