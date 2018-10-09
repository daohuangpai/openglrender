#pragma once
#include <GL/glew.h>
#include "util.h"
#include "mangertemplate.h"
#include "image.h"
#include <memory>
#include "../common/StringStream.h"

namespace GLUI {

	//扩展后  framebuffer需要改变

	class Texture
	{
	public:
		enum TextureTYPE {
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_3D,
			TEXTURE_CUBE,
			TEXTURE_ARRAY_2D

		};

		enum FilterParameter {
			MIN_FILTER = GL_TEXTURE_MIN_FILTER,
			MAG_FILTER = GL_TEXTURE_MAG_FILTER
		};

		//mip基层0上尺寸最大其他按二倍缩小，使用glGenerateMipmap直接mip到最小为2*2的尺寸
		enum FilterMode {
			NEAREST = GL_NEAREST,//在mip基层0上使用线性过滤
			LINEAR = GL_LINEAR,//在mip基层0上使用最邻近过滤
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,//选择最邻近的mip层，并使用最邻近过滤
			NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,//在mip层之间使用线性插值和最邻近过滤
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,//选择最邻近的mip层，使用线性过滤
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR//在mip层之间使用线性插值和使用线性过滤，又称三线性mipmap	
		};

		enum WrapParameter {
			WRAP_S,
			WRAP_T,
			WRAP_R
		};

		enum WrapMode {
			CLAMP = GL_CLAMP,
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER_ARB,
			REPEAT = GL_REPEAT,
			MIRROR = GL_MIRRORED_REPEAT_IBM
		};

		//setting  setting是指在创建图像纹理时需要设置的东西
		//GLenum Target = GL_TEXTURE_2D;//纹理理类型
		GLenum InternalFormat = GL_RGBA;//纹理图像源的图像格式
		GLenum Format = GL_RGBA;//要转成的纹理内部图像格式
		GLenum Type = GL_UNSIGNED_BYTE;//图像像素的表达类型，要将输入的图像变为float就只要设这个为GL_FLOAT就可以了

		FilterMode FilterMin = LINEAR_MIPMAP_NEAREST;//生成mip图缩小的算法
		FilterMode FilterMax = LINEAR;//生成mip图扩大的算法
									 //关于纹理的一维二维和三维其实就是类似一个查询表，对于一位纹理只有一个维度的插值查询，但是二维三维可能性更大
									 //在shader中类似tex(0.5)是一维的颜色，tex(0.5，0.6)二维的颜色，tex(0.5，0.6，0.7)三维的颜色。方式上一维图像插值只需要两个点，而二维四个点，三维八个点，
									 //这个是普通的线性插值的原理
		WrapMode WrapS = REPEAT;//纹理x轴拉伸的像素方法(0,1)纹理坐标，但是超出比如1.5或者-1.5的值的填充方式，大于等于1维纹理有用
		WrapMode WrapT = REPEAT;//纹理y轴拉伸的像素方法(0,1)纹理坐标，但是超出比如1.5或者-1.5的值的填充方式，大于等于2维纹理有用
		WrapMode WrapR = REPEAT;//纹理y轴拉伸的像素方法(0,1)纹理坐标，但是超出比如1.5或者-1.5的值的填充方式,仅3维纹理有用

		bool Mipmapping = true;
		//int maxlevel = -1;
		//也可以在bind到纹理后使用下面两个函数获得纹理的大小信息
		//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		GLsizei Width = 0;//x，大于等于1维纹理有用,在文件create时默认图像大小就是加载进来的图像大小
		GLsizei Height = 0;//y，大于等于2维纹理有用,在文件create时默认图像大小就是加载进来的图像大小
		GLsizei Depth = 0;//z,仅3维纹理有用,在文件create时默认图像大小就是加载进来的图像大小
		//				  //setting end

		static void getInfoFromNowBindTexture(GLenum target,Texture* texture);
		bool setFilterMode(FilterParameter param, FilterMode mode);
		bool setWrapMode(WrapParameter param, WrapMode mode);
		bool setInternalFormat(GLenum fomat);

		void Alignment(int width);
		bool ApplyParamter(GLenum target);
		int get_channle();

		bool Active(int unit);

		virtual void bind() = 0;
		virtual bool is_available() = 0;
		virtual void Unbind() = 0;
		virtual TextureTYPE TYPE() = 0;
		virtual GLenum Target() = 0;
		bool get_data(GLenum target, GLuint ID,OUT char** outdataptr);
		bool save_bmp(GLenum target, GLuint ID, const char* path);
	};


	class TextrueOne : public Texture {
	public:
		virtual ~TextrueOne() {glDeleteTextures(1, &m_handle); }
		inline void genhandleifnecessary() {
			if (!glIsTexture(m_handle))
				glGenTextures(1, &m_handle);
		}

		virtual bool is_available() override{ return glIsTexture(m_handle) == GL_TRUE;; }
		virtual void bind() override { glBindTexture(Target(), m_handle); }
		virtual void bind(unsigned int num) { Active(num); glBindTexture(Target(), m_handle); }
		virtual void Unbind() override { glBindTexture(Target(), 0); }
		GLuint ID() { return m_handle; }
	protected:
		GLuint m_handle;
	};


	class Texture1D :public TextrueOne {
	public:
		bool bind_data(char* data);
		virtual GLenum Target() override { return GL_TEXTURE_1D; }
		virtual TextureTYPE TYPE() override { return TEXTURE_1D; }

	};

	class Texture2D :public TextrueOne {
	public:
		bool bind_data(char* data);
		bool Copy(int level,char* data, int x,int y,int width, int height);
		virtual GLenum Target() override { return GL_TEXTURE_2D; }
		virtual TextureTYPE TYPE() override { return TEXTURE_2D; }
		static Texture2D* CreateTexture2D(GLUI::image*image);
		static Texture2D* CreateTexture2D(const char* path);
	};

	class Texture3D :public TextrueOne {
	public:
		bool bind_data(char* data);
		virtual GLenum Target() override { return GL_TEXTURE_3D; }
		virtual TextureTYPE TYPE() override { return TEXTURE_3D; }
	};

	class Texture2dArray : public TextrueOne {
	public:

		virtual TextureTYPE TYPE() override { return TEXTURE_ARRAY_2D; }
		virtual GLenum Target() override { return GL_TEXTURE_2D_ARRAY; }
		bool update_data(char* data, GLenum fomat, int width, int height, int index);
		bool bind_data(std::vector<std::shared_ptr<image>>& images);
		int size() {return  m_size;}
	private:
		int m_size  = 0;
	};

	class cubeTexture : public TextrueOne {
	public:
		enum CUBE_TEXTURET_FACE{
			RIGHT = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			LEFT = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			TOP = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			BOTTOM = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			FRONT = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			BACK = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};
		struct cubeInfoStruct {
			CUBE_TEXTURET_FACE face;
			char* data;
			int width;
			int height;
			GLenum InternalFormat;//纹理图像源的图像格式GL_RGBA等，使用std::initializer_list的参数不能有预先赋值，即不能这里写InternalFormat = GL_RGBA
		};

		//注意这个纹理和其他的不一样，只能支持64、512这种2的次幂的图像，否则报参数错误
		bool bind_data(std::initializer_list<cubeInfoStruct> cubeInfoStructs);
		virtual GLenum Target() override { return GL_TEXTURE_CUBE_MAP; }
		virtual TextureTYPE TYPE() override { return TEXTURE_CUBE; }
	};

	MangerTemplateClassPtr(std::string, Texture, texturemanger)
	MangerTemplateClassPtr(std::string, Texture2D, textur2demanger)
}