#pragma once
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <string>
#include "buffer.h"
#include "Texture.h"
#include "shader.h"
#include "util.h"

namespace GLUI
{



	class Vao {
	public:
		virtual~Vao() { if (glIsVertexArray(m_containerVAO)) glDeleteVertexArrays(1, &m_containerVAO); }
		inline void generate()
		{
			if (!is_available())
				glGenVertexArrays(1, &m_containerVAO);
			 
		}

		//旧管线的画矩形等这边glDrawArrays取消了
		enum DROW_TYPE
		{
			POINTS =GL_POINTS,
			LINES= GL_LINES,
			LINE_LOOP=GL_LINE_LOOP,
			LINE_STRIP =GL_LINE_STRIP,
			LINES_ADJACENCY = GL_LINES_ADJACENCY,
			LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
			TRIANGLES = GL_TRIANGLES,
			TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
			TRIANGLE_FAN = GL_TRIANGLE_FAN,
			TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
			TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY
		};
		enum DROW_ELEMENT_DATA_TYPE
		{
			UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
			UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
			UNSIGNED_INT = GL_UNSIGNED_INT
		};

		struct VaoBindBufferStruct {
			std::shared_ptr<buffer> buf;//
			unsigned int width;//取下一行的数据量，如果width = 0表明数据是紧密对齐的
			GLuint  layoutindex;//glsl layout num
			GLint  size;//根据DataType类型的个数，不是以byte为单位
			GLenum DataType;//指示数据的的类型
			GLboolean normalized;//是否把数据归一化
			unsigned int  offset;//对同一行数据偏移多少开始取，相对于sizeof(DataType)* widthsize
		};
		
		bool BindBuffer(std::initializer_list<VaoBindBufferStruct> VaoBindBufferStructs);
		
		void bind() { glBindVertexArray(m_containerVAO); }
		bool is_available() { return glIsVertexArray(m_containerVAO)==  GL_TRUE; }
		void Unbind() { glBindVertexArray(0); }

		void removeallbindbuffer() { m_bufferlist.clear(); }
		bool DrowArray(DROW_TYPE drowtype, int first, int num);
		bool Drawelements(buffer* elementbuffer,DROW_TYPE drowtype, GLsizei count​, DROW_ELEMENT_DATA_TYPE type​, const GLvoid * indices​);
		static unsigned int elementtypetobytesize(Vao::DROW_ELEMENT_DATA_TYPE type);
	private:
		bool autobindbufferbeforedrow();
		bool autobindbufferbeafterdrow();
	private:
		GLuint m_containerVAO;
		typedef std::map<GLuint, VaoBindBufferStruct> Layout_Buffer_Table;
		Layout_Buffer_Table m_bufferlist;//如果是映射到VAO的buffer生存将至少保证vao在的时候可以用
	};

	class Vaodynamic
	{
	public:
		inline void generate()
		{
			if (!is_available())
				glGenVertexArrays(1, &m_containerVAO);
		}
		void bind() { glBindVertexArray(m_containerVAO); }
		bool is_available() { return glIsVertexArray(m_containerVAO) == GL_TRUE; }
		void Unbind() { glBindVertexArray(0); }
		inline void enable_vertex(unsigned int num) { glEnableVertexAttribArray(num); }
		bool bindvectexbuffer(std::shared_ptr<buffer> buf,//
		                      unsigned int width,//取下一行的数据量，如果width = 0表明数据是紧密对齐的
		                      GLuint  layoutindex,//glsl layout num
		                      GLint  size,//根据DataType类型的个数，不是以byte为单位
		                      GLenum DataType,//指示数据的的类型
		                      GLboolean normalized,//是否把数据归一化
		                      unsigned int  offset//对同一行数据偏移多少开始取，相对于sizeof(DataType)* widthsize)
							  );
		inline void disabl_vertex(unsigned int num){glDisableVertexAttribArray(num);}
		bool DrowArray(Vao::DROW_TYPE drowtype, int first, int num);
		bool Drawelements(buffer* elementbuffer, Vao::DROW_TYPE drowtype, GLsizei count​, Vao::DROW_ELEMENT_DATA_TYPE type​, const GLvoid * indices​);
	private:
		GLuint m_containerVAO;
	};


	MangerTemplateClassPtr(std::string, Vao, Vaomanger)

}