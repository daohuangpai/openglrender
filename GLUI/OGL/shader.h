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
		GLenum glsl_value_type;//��ֵ�����ͣ�����GL_DOUBLE_MAT2x4��GL_DOUBLE��
		GLuint location;//������get_attrib����get_uniform�ķ���ֵһ�������ֵ��ID��ʶ
		int size;//�����ֵ��һ�����飬��������Ǵ�������Ĵ�С
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
		//����
		inline bool is_available() { return glIsProgram(m_program); }
		inline GLuint program() { return m_program; }
		inline void Use() { glUseProgram(m_program); }
		bool CheckValidate();//����Ƿ���Ч,�����Ч�����log������Ϣ
		GLint get_attrib(const char *name);//ֻ�ܳ����ڶ�����ɫ���У�vertex  shader ) ������ʾ��������
		
		//��������glsl�е�����
		/*
		layout (std140) uniform TransformBlock
		{
		  float scale;
		  vec3 pp[];
		}
		���ֽṹ��Ҫ��buffer��ʹ��UNIFORM_BUFFER������buffer����õ�����
		����ͨϵͳ�����uniformһ�������Ҳ���԰󶨣�ʹ��layout (std140��binding = num)��ֱ��ָ����ַ
		��ͨ������layout (location = num)

		����˵glBindBufferBase��һ��pointֻ��һ��ֵ�����ֵ����ӳ�䵽���block�������ֻ��Ҫʹ���������setUniformBlockIndex����Ӧ
		��ȻҲ����ֱ����glsl��дlayout (std140��binding = num)��ֱ��ָ����ַ

		Uniform Block index��Ŀ����Ϊ��ʹ������uniformBlock IDֵ���а󶨣����Uniform Block��IDֵ�����OpenGL Context�У�
		ͨ����������Ϊ����Uniform Block��UBO����������ͨ����Uniform Block��UBO�󶨵���ͬ��Binding points��ID����ʵ��UBO��Uniform Block �Ļ�����
		�����https://blog.csdn.net/csxiaoshui/article/details/32101977
	
		*/
		//blockname�Ǹû�������glsl�е����ƣ����������TransformBlock��Ȼ�����������ֱ�ӵ���scale
		//����TransformBlock.scale
		//�������vec3 pp[]����û�ж���������buffer��Ա������ͨ��length()��������pp.length()���ʵ�ʵ�������
		GLuint GetUniformBlockIndex(const char *blockname);
		bool bindUniformBlockIndexwithblockpoint(GLuint uniformBlockIndex, GLuint uniformBlockBinding);
		//�����������������
		bool set_uniform_block(const char *blockname, GLuint uniformBlockBinding);

		GLint get_uniform(const char *name);

		void get_program_all_attrib(std::vector<glsl_value>& list);//�õ�ȫ����attribֵ,���ֵ���ڶ�����ɫ������
		void get_program_all_uniform(std::vector<glsl_value>& list);///�õ�ȫ����uniformֵ


		void set_uniform(const char* valuename, glslvalue& value);

		void set_uniform_int(const char* valuename, GLint value);
		void set_uniform_int_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float(const char* valuename, GLfloat value);
		void set_uniform_Float_array(const char* valuename, GLsizei count, GLfloat* value);


		void set_uniform_int_vec2(const char* valuename, GLint value1, GLint value2);
		//��set_int_array��ͬ��������valueָ������ݽ���count*2����set_int_array��count*1
		void set_uniform_int_vec2_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float_vec2(const char* valuename, GLfloat value1, GLfloat value2);
		void set_uniform_float_vec2_array(const char* valuename, GLsizei count, GLfloat* value);

		void set_uniform_int_vec3(const char* valuename, GLint value1, GLint value2, GLint value3);
		//��set_int_array��ͬ��������valueָ������ݽ���count*3����set_int_array��count*1
		void set_uniform_int_vec3_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float_vec3(const char* valuename, GLfloat value1, GLfloat value2, GLfloat value3);
		void set_uniform_float_vec3_array(const char* valuename, GLsizei count, GLfloat* value);


		void set_uniform_int_vec4(const char* valuename, GLint value1, GLint value2, GLint value3, GLint value4);
		//��set_int_array��ͬ��������valueָ������ݽ���count*4����set_int_array��count*1
		void set_uniform_int_vec4_array(const char* valuename, GLsizei count, GLint* value);
		void set_uniform_float_vec4(const char* valuename, GLfloat value1, GLfloat value2, GLfloat value3, GLfloat value4);
		void set_uniform_float_vec4_array(const char* valuename, GLsizei count, GLfloat* value);

		//���һ����������value�費��Ҫת�����ݣ�һ�㲻Ҫ��,2*2��������ͬ��
		void set_uniform_mat_2_2(const char* valuename, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_3_3(const char* valuename, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_4_4(const char* valuename, GLfloat* value, GLboolean tran = GL_FALSE);


		void set_uniform_mat_2_2_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_3_3_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran = GL_FALSE);
		void set_uniform_mat_4_4_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran = GL_FALSE);


		//attrib̫����,һС���ֲ�����δ����
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


			//attrib̫����
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

		//�õ�
		//subroutine uniform veccolor veccolorModel;��ͨ���ֵ��ID
		inline GLint get_subroutine_uniform(SHADER_TYPE shadertype, const char *name)
		{
			GLint templateId = glGetSubroutineUniformLocation(m_program, shadertype, name);
			if (!opengl_error("get_subroutine_uniform failed")|| (templateId == -1)) {
				BOOST_LOG_TRIVIAL(error) << "name:"<< name;
				return -1;
			}		
			return templateId;
		}
		//�õ�
		//subroutine uniform veccolor veccolorModel���Ӻ�����ID
		inline GLuint get_subroutine_index(SHADER_TYPE shadertype, const char *name)
		{
			GLuint index = glGetSubroutineIndex(m_program, shadertype, name);
			if (!opengl_error("get_subroutine_index failed")|| (index == -1)) {
				BOOST_LOG_TRIVIAL(error) << "name:" << name;
				return -1;
			}
			return index;
		}

		//�õ����е��Ӻ������͵�uniform�ĸ�����-1�Ǵ���
		inline int get_uniform_subroutine_num(SHADER_TYPE shadertype)
		{
			GLsizei n;
			glGetProgramStageiv(m_program, shadertype, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &n);
			if (!opengl_error("sget_uniform_subroutine_num failed"))
				return -1;
			return n;
		}
		//����
		//����subroutine uniform veccolor veccolorModel���Ӻ�����ID���൱��ѡ����
		//��������������ã����û������ȫ�����Ӻ�������ô�������������ζ�������óɹ���
		inline bool set_uniform_subroutine_index(SHADER_TYPE shadertype, GLuint Index)
		{		
			glUniformSubroutinesuiv(shadertype, 1, &Index);
			return opengl_error("set_uniform_subroutine_index failed");
		}

	 

		//bool set_subroutine(SHADER_TYPE shadertype, char* templatename, char* funname);
		//�������ȫ�����ӳ�Ա������������κ�һ���Ӻ�����û�и�ֵ����ô�ͻ���opengl�������һ����������������ذ�ȫ����������
		bool set_subroutine(SHADER_TYPE shadertype, std::map<std::string,std::string>& funnames);

		//����subroutine��Uniform����ID�õ����ƣ�����
		//subroutine uniform veccolor veccolorModel;��ͨ��glGetSubroutineUniformLocation������uniform��IDֵ��Ȼ��������Եõ�����
		//ʹ��get_subroutine_uniform�õ�ֵ
		bool get_active_subroutine_uniform_name(SHADER_TYPE shadertype, GLuint Index,std::string& result);

		struct shader_creater_struct
		{
			SHADER_TYPE shadertype;//����
			char* param;//·����������
			bool isdata;//��ʾparam�ǲ������ݣ����Ǿ���·��
		};
		bool createshader(std::initializer_list<shader_creater_struct> shader_creater_structs, const std::function<bool(GLuint)> &beforelinkFuntion = [](GLuint)->bool {return true; });
		static GLuint create_shader_from_data(const char* source, GLenum type);
	protected:
		GLuint m_program = -1;
	};
	 

	class rendershader : public shader{
	public:
		
		virtual TYPE type() { return RENDER_SHADER; }
		//������������ʹ�ü�����ɫ��������webgl��opengles�ǲ���֧�ֵģ����һ�Ӱ��Ч��
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