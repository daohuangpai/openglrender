#pragma once
#include <GL/glew.h>
#include "util.h"
#include "mangertemplate.h"
#include "image.h"
#include <memory>
#include "../common/StringStream.h"

namespace GLUI {

	//��չ��  framebuffer��Ҫ�ı�

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

		//mip����0�ϳߴ����������������С��ʹ��glGenerateMipmapֱ��mip����СΪ2*2�ĳߴ�
		enum FilterMode {
			NEAREST = GL_NEAREST,//��mip����0��ʹ�����Թ���
			LINEAR = GL_LINEAR,//��mip����0��ʹ�����ڽ�����
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,//ѡ�����ڽ���mip�㣬��ʹ�����ڽ�����
			NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,//��mip��֮��ʹ�����Բ�ֵ�����ڽ�����
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,//ѡ�����ڽ���mip�㣬ʹ�����Թ���
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR//��mip��֮��ʹ�����Բ�ֵ��ʹ�����Թ��ˣ��ֳ�������mipmap	
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

		//setting  setting��ָ�ڴ���ͼ������ʱ��Ҫ���õĶ���
		//GLenum Target = GL_TEXTURE_2D;//����������
		GLenum InternalFormat = GL_RGBA;//����ͼ��Դ��ͼ���ʽ
		GLenum Format = GL_RGBA;//Ҫת�ɵ������ڲ�ͼ���ʽ
		GLenum Type = GL_UNSIGNED_BYTE;//ͼ�����صı�����ͣ�Ҫ�������ͼ���Ϊfloat��ֻҪ�����ΪGL_FLOAT�Ϳ�����

		FilterMode FilterMin = LINEAR_MIPMAP_NEAREST;//����mipͼ��С���㷨
		FilterMode FilterMax = LINEAR;//����mipͼ������㷨
									 //���������һά��ά����ά��ʵ��������һ����ѯ������һλ����ֻ��һ��ά�ȵĲ�ֵ��ѯ�����Ƕ�ά��ά�����Ը���
									 //��shader������tex(0.5)��һά����ɫ��tex(0.5��0.6)��ά����ɫ��tex(0.5��0.6��0.7)��ά����ɫ����ʽ��һάͼ���ֵֻ��Ҫ�����㣬����ά�ĸ��㣬��ά�˸��㣬
									 //�������ͨ�����Բ�ֵ��ԭ��
		WrapMode WrapS = REPEAT;//����x����������ط���(0,1)�������꣬���ǳ�������1.5����-1.5��ֵ����䷽ʽ�����ڵ���1ά��������
		WrapMode WrapT = REPEAT;//����y����������ط���(0,1)�������꣬���ǳ�������1.5����-1.5��ֵ����䷽ʽ�����ڵ���2ά��������
		WrapMode WrapR = REPEAT;//����y����������ط���(0,1)�������꣬���ǳ�������1.5����-1.5��ֵ����䷽ʽ,��3ά��������

		bool Mipmapping = true;
		//int maxlevel = -1;
		//Ҳ������bind�������ʹ���������������������Ĵ�С��Ϣ
		//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		GLsizei Width = 0;//x�����ڵ���1ά��������,���ļ�createʱĬ��ͼ���С���Ǽ��ؽ�����ͼ���С
		GLsizei Height = 0;//y�����ڵ���2ά��������,���ļ�createʱĬ��ͼ���С���Ǽ��ؽ�����ͼ���С
		GLsizei Depth = 0;//z,��3ά��������,���ļ�createʱĬ��ͼ���С���Ǽ��ؽ�����ͼ���С
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
			GLenum InternalFormat;//����ͼ��Դ��ͼ���ʽGL_RGBA�ȣ�ʹ��std::initializer_list�Ĳ���������Ԥ�ȸ�ֵ������������дInternalFormat = GL_RGBA
		};

		//ע���������������Ĳ�һ����ֻ��֧��64��512����2�Ĵ��ݵ�ͼ�񣬷��򱨲�������
		bool bind_data(std::initializer_list<cubeInfoStruct> cubeInfoStructs);
		virtual GLenum Target() override { return GL_TEXTURE_CUBE_MAP; }
		virtual TextureTYPE TYPE() override { return TEXTURE_CUBE; }
	};

	MangerTemplateClassPtr(std::string, Texture, texturemanger)
	MangerTemplateClassPtr(std::string, Texture2D, textur2demanger)
}