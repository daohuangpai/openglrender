#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "openglerror.h"
#include <boost/log/trivial.hpp>
#include "mangertemplate.h"
#include "../common/glslvalue.h"


#define USE_GEOMETRY_SHADER

namespace GLUI {

	struct glsl_value
	{
		std::string name;
		GLenum glsl_value_type;//该值的类型，比如GL_DOUBLE_MAT2x4、GL_DOUBLE等
		GLuint location;//和上面get_attrib或者get_uniform的返回值一样是这个值的ID标识
		int size;//如果该值是一个数组，则这里就是代表数组的大小
	};

	class shader {
	public:
		enum TYPE {
			RENDER_SHADER = 0,
			COMPUTE_SHADER,
			SHADER
		};

		enum SHADER_TYPE {
			VERTEX = GL_VERTEX_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			COMPUTE = GL_COMPUTE_SHADER,
			TESS_CONTROL = GL_TESS_CONTROL_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		};
		virtual ~shader() { reset(); }
		void reset();
		virtual TYPE type() { return SHADER; }
		//功能
		inline bool is_available() { return glIsProgram(m_program); }
		inline GLuint program() { return m_program; }
		inline void Use() { glUseProgram(m_program); }
		bool CheckValidate();//检测是否有效,如果无效则输出log错误信息
		GLint get_attrib(const char *name);//只能出现在顶点着色器中（vertex  shader ) 用来表示顶点数据
		
		//仅仅对于glsl中的类似
		/*
		layout (std140) uniform TransformBlock
		{
		  float scale;
		  vec3 pp[];
		}
		这种结构需要在buffer中使用UNIFORM_BUFFER创建的buffer这里得到坐标
		和普通系统定义的uniform一样，这边也可以绑定，使用layout (std140，binding = num)来直接指定地址
		普通的则是layout (location = num)

		就是说glBindBufferBase绑定一个point只是一个值，这个值可以映射到多个block，而这个只需要使用这个函数setUniformBlockIndex来对应
		当然也可以直接在glsl中写layout (std140，binding = num)来直接指定地址

		Uniform Block index的目的是为了使得它和uniformBlock ID值进行绑定，这个Uniform Block的ID值存放在OpenGL Context中，
		通过它可以作为连接Uniform Block和UBO的桥梁：（通过把Uniform Block和UBO绑定到相同的Binding points（ID）上实现UBO和Uniform Block 的互动）
		具体见https://blog.csdn.net/csxiaoshui/article/details/32101977
	
		*/
		//blockname是该缓冲区在glsl中的名称，在上面就是TransformBlock，然后里面的数据直接调用scale
		//不用TransformBlock.scale
		//另外对于vec3 pp[]这种没有定义完整的buffer成员，可以通过length()函数，即pp.length()获得实际的数据数
		GLuint GetUniformBlockIndex(const char *blockname);
		bool bindUniformBlockIndexwithblockpoint(GLuint uniformBlockIndex, GLuint uniformBlockBinding);
		//上面两个函数的组合
		bool set_uniform_block(const char *blockname, GLuint uniformBlockBinding);

		GLint get_uniform(const char *name);

		void get_program_all_attrib(std::vector<glsl_value>& list);//得到全部的attrib值,这个值仅在顶点着色器可用
		void get_program_all_uniform(std::vector<glsl_value>& list);///得到全部的uniform值


		void set_uniform(const char* valuename, glslvalue& value);

		void set_uniform_int(const char* valuename, GLint value);
		void set_uniform_int_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float(const char* valuename, GLfloat value);
		void set_uniform_Float_array(const char* valuename, GLsizei count, GLfloat* value);


		void set_uniform_int_vec2(const char* valuename, GLint value1, GLint value2);
		//和set_int_array不同的是这里value指向的数据将是count*2，而set_int_array是count*1
		void set_uniform_int_vec2_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float_vec2(const char* valuename, GLfloat value1, GLfloat value2);
		void set_uniform_float_vec2_array(const char* valuename, GLsizei count, GLfloat* value);

		void set_uniform_int_vec3(const char* valuename, GLint value1, GLint value2, GLint value3);
		//和set_int_array不同的是这里value指向的数据将是count*3，而set_int_array是count*1
		void set_uniform_int_vec3_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float_vec3(const char* valuename, GLfloat value1, GLfloat value2, GLfloat value3);
		void set_uniform_float_vec3_array(const char* valuename, GLsizei count, GLfloat* value);


		void set_uniform_int_vec4(const char* valuename, GLint value1, GLint value2, GLint value3, GLint value4);
		//和set_int_array不同的是这里value指向的数据将是count*4，而set_int_array是count*1
		void set_uniform_int_vec4_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float_vec4(const char* valuename, GLfloat value1, GLfloat value2, GLfloat value3, GLfloat value4);
		void set_uniform_float_vec4_array(const char* valuename, GLsizei count, GLfloat* value);

		//最后一个参数描述value需不需要转置数据，一般不要吧,2*2矩阵，下面同理
		void set_uniform_mat_2_2(const char* valuename, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_3_3(const char* valuename, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_4_4(const char* valuename, GLfloat* value, GLboolean tran = GL_FALSE);


		void set_uniform_mat_2_2_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_3_3_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_4_4_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran = GL_FALSE);


		//attrib太多了,一小部分不常用未加入
#define ATTRIB_SET_FUN_2_PARAM(funname,paramtype,openglfun) \
		void set_attrib_##funname##(const char* valuename, paramtype value)\
		{\
			int locid = get_attrib(valuename);\
			if (locid >= 0)\
				openglfun(locid, value);\
		}


#define  ATTRIB_ARRAY_FUN(type,paramtype,funcbegin,funend)\
                ATTRIB_SET_FUN_2_PARAM(##type##_array, paramtype, ##funcbegin##1##funend##)\
		        ATTRIB_SET_FUN_2_PARAM(##type##_vec2_array, paramtype, ##funcbegin##2##funend##)\
			    ATTRIB_SET_FUN_2_PARAM(##type##_vec3_array, paramtype,##funcbegin##3##funend##)\
			    ATTRIB_SET_FUN_2_PARAM(##type##_vec4_array, paramtype,##funcbegin##4##funend##)


		//like ATTRIB_ARRAY_FUN(float,const GLfloat*,fv) =
		/*
		ATTRIB_SET_FUN_2_PARAM(float_array, const  GLfloat*, glVertexAttrib1fv)
		ATTRIB_SET_FUN_2_PARAM(float_vec2_array, const GLfloat*, glVertexAttrib2fv)
		ATTRIB_SET_FUN_2_PARAM(float_vec3_array, const GLfloat*, glVertexAttrib3fv)
		ATTRIB_SET_FUN_2_PARAM(float_vec4_array, const GLfloat*, glVertexAttrib4fv)
		*/
		ATTRIB_ARRAY_FUN(float, const GLfloat*, glVertexAttrib, fv)
			ATTRIB_ARRAY_FUN(short, const GLshort *, glVertexAttrib, sv)
			ATTRIB_ARRAY_FUN(double, const GLdouble  *, glVertexAttrib, dv)
			ATTRIB_ARRAY_FUN(int, const GLint *, glVertexAttribI, iv)
			ATTRIB_ARRAY_FUN(uint, const GLuint *, glVertexAttribI, uiv)


			//attrib太多了
#define ATTRIB_SET_FUN_USE_PARAM(funname,paramtype,funcbegin,funend) \
		void set_attrib_##funname##(const char* valuename, paramtype value)\
		{\
			int locid = get_attrib(valuename);\
			if (locid >= 0)\
				funcbegin##1##funend##(locid, value);\
		}\
       void set_attrib_##funname##_vec2(const char* valuename, paramtype value1, paramtype value2)\
		{\
			int locid = get_attrib(valuename);\
			if (locid >= 0)\
				funcbegin##2##funend##(locid, value1,value2);\
		}\
        void set_attrib_##funname##_vec3(const char* valuename, paramtype value1, paramtype value2,paramtype value3)\
		{\
			int locid = get_attrib(valuename);\
			if (locid >= 0)\
				funcbegin##3##funend##(locid, value1,value2,value3);\
		}\
        void set_attrib_##funname##_vec4(const char* valuename, paramtype value1, paramtype value2,paramtype value3,paramtype value4)\
		{\
			int locid = get_attrib(valuename);\
			if (locid >= 0)\
				funcbegin##4##funend##(locid, value1,value2,value3,value4);\
		}

			ATTRIB_SET_FUN_USE_PARAM(float, GLfloat, glVertexAttrib, f)
			ATTRIB_SET_FUN_USE_PARAM(short, GLshort, glVertexAttrib, s)
			ATTRIB_SET_FUN_USE_PARAM(double, GLdouble, glVertexAttrib, d)
			ATTRIB_SET_FUN_USE_PARAM(int, GLint, glVertexAttribI, i)
			ATTRIB_SET_FUN_USE_PARAM(uint, GLuint, glVertexAttribI, ui)

		//得到
		//subroutine uniform veccolor veccolorModel;，通这个值的ID
		inline GLint get_subroutine_uniform(SHADER_TYPE shadertype, const char *name)
		{
			GLint templateId = glGetSubroutineUniformLocation(m_program, shadertype, name);
			if (!opengl_error("get_subroutine_uniform failed")|| (templateId == -1)) {
				BOOST_LOG_TRIVIAL(error) << "name:"<< name;
				return -1;
			}		
			return templateId;
		}
		//得到
		//subroutine uniform veccolor veccolorModel的子函数的ID
		inline GLuint get_subroutine_index(SHADER_TYPE shadertype, const char *name)
		{
			GLuint index = glGetSubroutineIndex(m_program, shadertype, name);
			if (!opengl_error("get_subroutine_index failed")|| (index == -1)) {
				BOOST_LOG_TRIVIAL(error) << "name:" << name;
				return -1;
			}
			return index;
		}

		//得到其中的子函数类型的uniform的个数，-1是错误
		inline int get_uniform_subroutine_num(SHADER_TYPE shadertype)
		{
			GLsizei n;
			glGetProgramStageiv(m_program, shadertype, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &n);
			if (!opengl_error("sget_uniform_subroutine_num failed"))
				return -1;
			return n;
		}
		//设置
		//设置subroutine uniform veccolor veccolorModel的子函数的ID，相当于选择函数
		//这个函数并不能用，如果没有设置全部的子函数，那么这个函数无论如何都不会调用成功的
		inline bool set_uniform_subroutine_index(SHADER_TYPE shadertype, GLuint Index)
		{		
			glUniformSubroutinesuiv(shadertype, 1, &Index);
			return opengl_error("set_uniform_subroutine_index failed");
		}

	 

		//bool set_subroutine(SHADER_TYPE shadertype, char* templatename, char* funname);
		//这边设置全部的子成员函数，如果有任何一个子函数并没有赋值，那么就会在opengl里面产生一个错误，所以这里务必把全部包含进来
		bool set_subroutine(SHADER_TYPE shadertype, std::map<std::string,std::string>& funnames);

		//根据subroutine的Uniform名的ID得到名称，比如
		//subroutine uniform veccolor veccolorModel;，通过glGetSubroutineUniformLocation获得这个uniform的ID值，然后这里可以得到名称
		//使用get_subroutine_uniform得到值
		bool get_active_subroutine_uniform_name(SHADER_TYPE shadertype, GLuint Index,std::string& result);

		struct shader_creater_struct
		{
			SHADER_TYPE shadertype;//类型
			char* param;//路径或者数据
			bool isdata;//表示param是不是数据，不是就是路径
		};
		bool createshader(std::initializer_list<shader_creater_struct> shader_creater_structs, const std::function<bool(GLuint)> &beforelinkFuntion = [](GLuint)->bool {return true; });
		static GLuint create_shader_from_data(const char* source, GLenum type);
	protected:
		GLuint m_program = -1;
	};
	 

	class rendershader : public shader{
	public:
		
		virtual TYPE type() { return RENDER_SHADER; }
		//下面两个函数使用几何着色器，这在webgl和opengles是不被支持的，并且会影响效率
#ifdef USE_GEOMETRY_SHADER
		void create_program_from_data(const char* vertexdata, const char *geometrydata, const char *fragmentdata, const std::function<bool(GLuint,GLuint, GLuint, GLuint)> &beforelinkFuntion = [](GLuint, GLuint, GLuint, GLuint)->bool {return true;});
		void create_program_from_file(const char* vertexpath, const char *geometrypath, const char *fragmentpath, const std::function<bool(GLuint, GLuint, GLuint, GLuint)> &beforelinkFuntion = [](GLuint, GLuint, GLuint, GLuint)->bool {return true; });
		void create_program_from_file_extension(const char* vertexpath, const char *geometrypath, const char *fragmentpath, const std::function<bool(GLuint, GLuint, GLuint, GLuint)> &beforelinkFuntion = [](GLuint, GLuint, GLuint, GLuint)->bool {return true; });

#else 
		void create_program_from_data(const char* vertexdata, const char *fragmentdata);
		void create_program_from_file(const char* vertexpath, const char *fragmentpath);
#endif // USE_GEOMETRY_SHADER
	};
	MangerTemplateClassPtr(std::string, rendershader, rendershadermanger)


	class computershader : public shader 
	{
	  public:
		virtual TYPE type() { return COMPUTE_SHADER; }
		void create_compute_program_from_file(const char* computepath);
		void create_compute_program_from_file_extension(const char* computepath);
		void create_compute_program_from_data(const char* computedata);
		bool get_max_compute_work_group_size(int& x, int& y, int& z);
	  private:
        
	};
	MangerTemplateClassPtr(std::string, computershader, computershadermanger)

	MangerTemplateClassPtr(std::string,shader, shadermanger)

}