#include "convolution.h"
#include "../OGL/util.h"
#include "../OGL/shader.h"
#include "../OGL/FrameBuffer.h"
#include "../OGL/buffer.h"
#include "../OGL/vao.h"
#include "../OGL/commonbuffer.h"
#include "assembly.h"

namespace GLUI
{

	const char convolutionvectex[] = { MAKE_TEXT(
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

	
	const char convolutionfragment[] = {
		MAKE_TEXT(
			#version 440 core\n
			layout(std140, binding = 0) buffer kernelbuffer\n
	       { \n
			float kernel[]; \n
	       }; \n
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
		     vec3 result = vec3(0,0,0);\n
	         int beginx = -(kernelwidth - 1) / 2; \n
		     int beginy = -(kernelheight - 1) / 2; \n
		     for (int j = 0; j < kernelheight; j++)\n
		     { \n
			     for (int i = 0; i < kernelwidth; i++)\n
			     { \n
				    vec2 tex = vec2(texcoord.x + (beginx + i)*DeltaX, texcoord.y + (beginy + j)*DeltaY); \n
				    result = result + texture2D(process_image, tex).xyz*kernel[j*kernelwidth + i]; \n
			     }\n
		      }\n
			return result; \n
	       }

		    void main(void)
		   {
		     color = vec4(convolution(), 1.0);
		    }
    	)
	};


	const char convolutioncomputerchar[] = {
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
			     ivec2 bv = ivec2(x,y); \n
			     result = result + imageLoad(input_image, bv.xy)*kernel[j*kernelwidth + i]; \n
		       }\n
	         }\n
		      imageStore(output_image, texcoord.xy, result); \n
	      }
		)
	};

	 

	std::shared_ptr<Texture2D> convolution(IN std::shared_ptr<Texture2D>& src_tex, IN dataarray2D<float>& kernel)
	{
		std::shared_ptr<GLUI::shader> sdr;
		if (!shadermanger::shareshadermanger()->get(std::string("GLUI_COVVOLUTION"), sdr))
		{
			sdr.reset(new GLUI::shader);
			if (!sdr->createshader({ { GLUI::shader::VERTEX,(char*)convolutionvectex,true },
			{ GLUI::shader::FRAGMENT,(char*)convolutionfragment,true } })){
				return nullptr;
			}
			shadermanger::shareshadermanger()->set(std::string("GLUI_COVVOLUTION"), sdr);
		}

		std::shared_ptr<GLUI::Vao> vao = get_image_process_vao();
		if (!vao.get())
			return nullptr;
		std::shared_ptr<Texture2D> outimagetexture(new Texture2D);	
		outimagetexture->Height = src_tex->Height;
		outimagetexture->Width = src_tex->Width;
		outimagetexture->setInternalFormat(src_tex->InternalFormat);
		outimagetexture->bind_data(nullptr);
		std::shared_ptr<GLUI::framebuffer> framebuffer = get_image_process_framebuffer(outimagetexture);
		if (!framebuffer.get())
			return nullptr;

		float deltax = 1.0f / src_tex->Width;
		float deltay = 1.0f / src_tex->Height;
		int kernelwidth = kernel.width();
		int kernelheight = kernel.height();

		int totalsize = kernelwidth*kernelheight;
		std::shared_ptr<GLUI::buffer> kernelbuffer(new GLUI::buffer());
		if (!SimpleBufferArray<float>::generatebuffer(totalsize * 4,
			GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, kernelbuffer))
		{
			return nullptr;
		}
		SimpleBufferArray<float> kernelarray;
		kernelarray.bindbuffer(kernelbuffer, 0, totalsize * 4);
		kernelarray.AccessData([&](float* data, int size)->bool
		{
			for (int j = 0; j < kernelheight; j++)
			{
				for (int i = 0; i < kernelwidth; i++)
				{
					data[j * kernelwidth * 4 + i * 4] = kernel[j][i];
				}
			}
			return true;
		}
		);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, src_tex->Width, src_tex->Height);
		framebuffer->Bind();
		src_tex->bind();
		kernelbuffer->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER, 0, 0, kernelarray.bytesize());
		vao->bind();
		sdr->Use();
		sdr->set_uniform("kernelwidth", GLUI::glslvalue(kernelwidth));
		sdr->set_uniform("kernelheight", GLUI::glslvalue(kernelheight));
		sdr->set_uniform("DeltaY", GLUI::glslvalue(deltay));
		sdr->set_uniform("DeltaX", GLUI::glslvalue(deltax));
		vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
		framebuffer->Unbind();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		return outimagetexture;
	}



	const char grayvectex[] = { MAKE_TEXT(
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


	const char grayfragment[] = {
		MAKE_TEXT(
			#version 440 core\n
			layout(std140, binding = 0) buffer kernelbuffer\n
	      { \n
			float kernel[]; \n
	      }; \n
			//uniform float kernel[25];
			uniform sampler2D process_image; \n
			in vec2 texcoord; \n
			out vec4 color; \n
			void main(void)
			{
	        	vec3 imgcolor = texture2D(process_image, texcoord).xyz;
		        float value = imgcolor[0]*0.299 + imgcolor[1]*0.587 + imgcolor[2]*0.114;
				color = vec4(vec3(value), 1.0);
			}
	)
	};
	std::shared_ptr<Texture2D> gray(IN std::shared_ptr<Texture2D>& src_tex)
	{
		std::shared_ptr<GLUI::shader> sdr;
		if (!shadermanger::shareshadermanger()->get(std::string("GLUI_GRAY"), sdr))
		{
			sdr.reset(new GLUI::shader);
			if (!sdr->createshader({ { GLUI::shader::VERTEX,(char*)grayvectex,true },
			{ GLUI::shader::FRAGMENT,(char*)grayfragment,true } })) {
				return nullptr;
			}
			shadermanger::shareshadermanger()->set(std::string("GLUI_GRAY"), sdr);
		}

		std::shared_ptr<GLUI::Vao> vao = get_image_process_vao();
		if (!vao.get())
			return nullptr;
	
		std::shared_ptr<Texture2D> outimagetexture(new Texture2D);

		outimagetexture->Height = src_tex->Height;
		outimagetexture->Width = src_tex->Width;
		outimagetexture->setInternalFormat(src_tex->InternalFormat);
		outimagetexture->bind_data(nullptr);
		std::shared_ptr<GLUI::framebuffer> framebuffer = get_image_process_framebuffer(outimagetexture);
		if (!framebuffer.get())
			return nullptr;


		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, src_tex->Width, src_tex->Height);
		framebuffer->Bind();
		src_tex->bind();
		vao->bind();
		sdr->Use();
		vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 4);
		framebuffer->Unbind();
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		return outimagetexture;
	}

}