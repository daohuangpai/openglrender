#pragma once
#include "mangertemplate.h"
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

namespace GLUI {

	
	class image {
	public:
		int m_width = 0;
		int m_height = 0;
		int m_channle = 0;
		int m_bitperPixel = 0;
		unsigned char * m_data = nullptr;

		unsigned int m_capacity = 0;
		void image_malloc(unsigned int size);
		virtual~image() { 
			if (m_data) 
				free(m_data);
		}
		virtual bool generate(const char* path);
		virtual bool generateempty(int width,int height,int channle);
		bool is_available() { return m_data != nullptr; }
		GLenum OpengType();
		static std::shared_ptr<image> GlobalGet(const char* path);
	};
 
	MangerTemplateClassPtr(std::string, image, imagemanger)


	template<class type>
	class imagedata {
	public:
		int m_width = 0;
		int m_height = 0;
		int m_channle = 0;
		type * m_data = nullptr;

		unsigned int m_capacity = 0;//以byte为单位的个数
		unsigned int m_size = 0;//以type为单位的个数

		bool is_available() { return m_data != nullptr; }

		void generateempty(int width, int height,int channle)
		{
			int size = width*height*channle;
			image_malloc(size);
			m_width = width;
			m_height = height;
			m_channle = channle;
		}
		void image_malloc(unsigned int size)
		{
			int realsize = size * sizeof(type);
			if (!is_available()) {
				m_data = (type *)malloc(realsize);
				m_capacity = realsize;
			}
			else if (m_capacity < realsize) {
				while (m_capacity < realsize)
					m_capacity = m_capacity << 1;
				m_data = (type *)realloc(m_data, m_capacity);
			}
			m_size = size;
		}
		virtual~imagedata() {
			if (m_data)
				free(m_data);
		}

	

	};

}