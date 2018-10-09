#include "image.h"
#include "util.h"
#include <boost/log/trivial.hpp>
//使用之前宏定义来使文件中函数生效
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace GLUI {

	struct DepthPixelRelations
	{
		int depth;
		int m_bitperPixel;
	};

	DepthPixelRelations sizedDepthPixelRelations[] = {
		{1,8},
		{3,24},
		{4,32}
	};


	MangerTemplateClassCPP(imagemanger)

	bool image::generate(const char* path)
	{
		int width = 0, height = 0, nrComponents = 0;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		auto stdfree = GLUI::MakeGuard(data, stbi_image_free);//保证删除;
		if ((width == 0) || (height == 0))
		{
			BOOST_LOG_TRIVIAL(error) << "faile to load image file" << path;
			if (data)
				BOOST_LOG_TRIVIAL(error) << "more info:" << data;
			return false;
		}


		int tablesize = sizeof(sizedDepthPixelRelations) / sizeof(DepthPixelRelations);
		int bitperPixel = 0;
		for (int i = 0; i < tablesize; i++)
		{
			if (sizedDepthPixelRelations[i].depth == nrComponents) {
				bitperPixel = sizedDepthPixelRelations[i].m_bitperPixel;
			}
		}
		if (bitperPixel == 0) {
			BOOST_LOG_TRIVIAL(error) << "only support 1 3 4 depth image:" << path;
			return false;
		}

		int size = width*height*nrComponents;
		//m_data = (unsigned char *)malloc(size);
		image_malloc(size);
		memcpy(m_data,data, size);
		m_width = width;
		m_height = height;
		m_channle = nrComponents;
		return true;
	}

	void image::image_malloc(unsigned int size)
	{
		if (!is_available()) {
			m_data = (unsigned char *)malloc(size);
			m_capacity = size;
		}
		else if (m_capacity < size) {
			while (m_capacity < size)
				m_capacity = m_capacity << 1;
			m_data = (unsigned char *)realloc(m_data, m_capacity);
		}
	}

	bool image::generateempty(int width, int height, int channle)
	{
		int size = width*height*channle;
		image_malloc(size);
		m_width = width;
		m_height = height;
		m_channle = channle;
		return is_available();
	}

	std::shared_ptr<image> image::GlobalGet(const char* path)
	{
		std::shared_ptr<image> img;
		if (!imagemanger::shareimagemanger()->get(std::string(path), img))
		{	
			img.reset(new image());
			if (img->generate(path)) {
				imagemanger::shareimagemanger()->set(std::string(path), img);
			}
		}
		return img;
	}

	GLenum image::OpengType()
	{
		switch (m_channle)
		{
		case 1:return GL_LUMINANCE; break;
		case 3:return GL_RGB;  break;
		case 4:return GL_RGBA; break;
		}
		BOOST_LOG_TRIVIAL(error) << "OpengType type???";
		return -1;
	}



}