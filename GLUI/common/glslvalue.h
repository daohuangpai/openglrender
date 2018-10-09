#pragma once
#include <GL/glew.h>
#include "windows.h"
#include <assert.h>
#include "../OGL/util.h"
#include "../OGL/treestruct.h"

namespace GLUI
{
	class glslvalue {
	public:
		enum TYPE
		{
			GLSL_INT = 0,
			GLSL_INT_VEC2,
			GLSL_INT_VEC3,
			GLSL_INT_VEC4,
			GLSL_INT_ARRAY,
			GLSL_INT_VEC2_ARRAY,
			GLSL_INT_VEC3_ARRAY,
			GLSL_INT_VEC4_ARRAY,	 
			GLSL_FLOAT ,
			GLSL_FLOAT_VEC2,
			GLSL_FLOAT_VEC3,
			GLSL_FLOAT_VEC4,
			GLSL_FLOAT_MAT2,
			GLSL_FLOAT_MAT3,
			GLSL_FLOAT_MAT4,
			GLSL_FLOAT_ARRAY,
			GLSL_FLOAT_VEC2_ARRAY,
			GLSL_FLOAT_VEC3_ARRAY,
			GLSL_FLOAT_VEC4_ARRAY,
			GLSL_FLOAT_MAT2_ARRAY,
			GLSL_FLOAT_MAT3_ARRAY,
			GLSL_FLOAT_MAT4_ARRAY,
			GLSL_NULL
		};

		virtual~ glslvalue() { malloc_release(); }
#define makeglslvalue(TYPE)\
        glslvalue(TYPE type){*this = type;}

		glslvalue() {}
		makeglslvalue(int)
		makeglslvalue(float)
		makeglslvalue(glm::ivec2)
		makeglslvalue(glm::ivec3)
		makeglslvalue(glm::ivec4)
		makeglslvalue(glm::vec2)
		makeglslvalue(glm::vec3)
		makeglslvalue(glm::vec4)
		makeglslvalue(glm::mat2)
		makeglslvalue(glm::mat3)
		makeglslvalue(glm::mat4)

		void malloc_incase(unsigned int size)
		{
			int realsize = size;
			if (m_data == nullptr) {
				m_data = malloc(realsize);
				m_capacity = realsize;
			}
			else if (m_capacity < realsize) {
				while (m_capacity < realsize)
					m_capacity = m_capacity << 1;
				m_data = realloc(m_data, m_capacity);
			}
			m_size = size;
		}
		void malloc_release()
		{
			if (m_data != nullptr) {
				free(m_data);
			}
		}

#define NO_BUFFER_OPERATOR_FUN(TYPE,ENUMNAME,unvalue,getname) \
       glslvalue& operator=(const TYPE v) \
	   {\
		unvalue = v;\
		m_type = ENUMNAME;\
		return *this;\
	   }\
      TYPE& As_##getname##()\
	  {\
		assert(m_type == ENUMNAME);\
		return *(TYPE*)(&glint);\
	  }\
      bool is_##getname##()\
	  {\
			return (m_type == ENUMNAME);\
	  }

		NO_BUFFER_OPERATOR_FUN(GLint, GLSL_INT, glint, int)
		NO_BUFFER_OPERATOR_FUN(GLfloat, GLSL_FLOAT, glfloat, float)

#define HAVE_BUFFER_OPERATOR_FUN(TYPE,ENUMNAME,getname) \
       glslvalue& operator=(const TYPE v) \
	   {\
		unsigned int size = sizeof(TYPE);\
        malloc_incase(size);\
		m_type = ENUMNAME;\
        memcpy(m_data,&v,size);\
        data = m_data;\
		return *this;\
	   }\
      TYPE& As_##getname##()\
	  {\
		assert(m_type == ENUMNAME);\
		return *static_cast<TYPE*>(data);\
	  }\
      bool is_##getname##()\
	  {\
			return (m_type == ENUMNAME);\
	  }

		HAVE_BUFFER_OPERATOR_FUN(glm::ivec2, GLSL_INT_VEC2, ivec2)
		HAVE_BUFFER_OPERATOR_FUN(glm::ivec3, GLSL_INT_VEC3, ivec3)
		HAVE_BUFFER_OPERATOR_FUN(glm::ivec4, GLSL_INT_VEC4, ivec4)
		HAVE_BUFFER_OPERATOR_FUN(glm::vec2, GLSL_FLOAT_VEC2, fvec2)
		HAVE_BUFFER_OPERATOR_FUN(glm::vec3, GLSL_FLOAT_VEC3, fvec3)
		HAVE_BUFFER_OPERATOR_FUN(glm::vec4, GLSL_FLOAT_VEC4, fvec4)
		HAVE_BUFFER_OPERATOR_FUN(glm::mat2, GLSL_FLOAT_MAT2, fmat2)
		HAVE_BUFFER_OPERATOR_FUN(glm::mat3, GLSL_FLOAT_MAT3, fmat3)
		HAVE_BUFFER_OPERATOR_FUN(glm::mat4, GLSL_FLOAT_MAT4, fmat4)

#define HAVE_BUFFER_ARRAY_FUN(TYPE,ENUMNAME,getname) \
       glslvalue& set##getname##(const TYPE* v,int len) \
	   {\
		unsigned int size = sizeof(TYPE)*len;\
        malloc_incase(size);\
		m_type = ENUMNAME;\
        memcpy(m_data,&v,size);\
        data = m_data;\
		return *this;\
	   }\
      TYPE* As_##getname##()\
	  {\
		assert(m_type == ENUMNAME);\
		return static_cast<TYPE*>(data);\
	  }\
      unsigned int get_##getname##_size()\
      {\
			return m_size / sizeof(TYPE);\
	   }\
       bool is_##getname##()\
	  {\
			return (m_type == ENUMNAME);\
	  }

		HAVE_BUFFER_ARRAY_FUN(GLint,GLSL_INT_ARRAY,intarray)
		HAVE_BUFFER_ARRAY_FUN(glm::ivec2, GLSL_INT_VEC2_ARRAY, intvec2array)
		HAVE_BUFFER_ARRAY_FUN(glm::ivec3, GLSL_INT_VEC3_ARRAY, intvec3array)
		HAVE_BUFFER_ARRAY_FUN(glm::ivec4, GLSL_INT_VEC4_ARRAY, intvec4array)

		HAVE_BUFFER_ARRAY_FUN(glm::vec2, GLSL_FLOAT_VEC2_ARRAY, floatvec2array)
		HAVE_BUFFER_ARRAY_FUN(glm::vec3, GLSL_FLOAT_VEC3_ARRAY, floatvec3array)
		HAVE_BUFFER_ARRAY_FUN(glm::vec4, GLSL_FLOAT_VEC4_ARRAY, floatvec4array)

		HAVE_BUFFER_ARRAY_FUN(glm::mat2, GLSL_FLOAT_MAT2_ARRAY, floatmat2array)
		HAVE_BUFFER_ARRAY_FUN(glm::mat3, GLSL_FLOAT_MAT3_ARRAY, floatmat3array)
		HAVE_BUFFER_ARRAY_FUN(glm::mat4, GLSL_FLOAT_MAT4_ARRAY, floatmat4array)

		TYPE type() { return m_type; }
	private:
		union 
		{
			GLint glint;
			void* data;
			GLfloat glfloat;
		};
		TYPE m_type = GLSL_NULL;

		unsigned int m_size = 0;
		unsigned int m_capacity = 0;//以byte为单位的个数
		void* m_data = nullptr;
	};

	typedef std::map<std::string, glslvalue> glslconfigure;
#define GLSL_POSITION "vectex"

}