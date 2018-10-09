#include "shader.h"
#include "util.h"
#include "..\common\StringStream.h"
#include "openglerror.h"

#pragma warning(disable:4996)

namespace GLUI
{
	//仅读文本文件
	char* file_read(const char* filename) {
		FILE* in = fopen(filename, "rb");
		if (in == NULL)
			return NULL;
		int res_size = BUFSIZ;
		char* res = (char*)malloc(res_size);
		int nb_read_total = 0;
		while (!feof(in) && !ferror(in)) {
			if (nb_read_total + BUFSIZ > res_size) {
				res_size = res_size << 2;
				res = (char*)realloc(res, res_size);
			}
			char* p_res = res + nb_read_total;
			nb_read_total += fread(p_res, 1, BUFSIZ, in);
		}
		fclose(in);
		res = (char*)realloc(res, nb_read_total + 1);
		res[nb_read_total] = '\0';
		return res;
	}

	bool file_read(const char* filename,std::string& data) {
		char* str = file_read(filename);
		if (!str) {
			return false;
		}
		data = str;
		free(str);
		return true;
	}

	void print_log(GLuint object) {
		GLint log_length = 0;
		if (glIsShader(object))
			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
		else if (glIsProgram(object))
			glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
		else {
			BOOST_LOG_TRIVIAL(error) << "printlog: Not a shader or a program\n";
			return;
		}
		char* log = (char*)malloc(log_length);
		if (glIsShader(object))
			glGetShaderInfoLog(object, log_length, NULL, log);
		else if (glIsProgram(object))
			glGetProgramInfoLog(object, log_length, NULL, log);
		BOOST_LOG_TRIVIAL(error) << log;
		free(log);
	}


	GLuint shader::create_shader_from_data(const char* source, GLenum type)
	{
		if (source == nullptr) {
			BOOST_LOG_TRIVIAL(error) << "create_shader with no data";
			return -1;
		}

		GLuint res = glCreateShader(type);
		const GLchar* sources[] = { source };
		glShaderSource(res, 1, sources, NULL);
		glCompileShader(res);
		GLint compile_ok = GL_FALSE;
		glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
		if (compile_ok == GL_FALSE) {
			print_log(res);
			glDeleteShader(res);
			return -1;
		}
		return res;
	}

#ifdef USE_GEOMETRY_SHADER

	GLuint create_program_with_fun(const char* vertexdata, const char* geometrydata, const char *fragmentdata, const std::function<bool(GLuint,GLuint, GLuint, GLuint)> &beforelinkFuntion)
	{
		GLuint program = glCreateProgram();
		GLuint vshader = -1, fshader = -1, gshader = -1;
		if (vertexdata) {
			vshader = shader::create_shader_from_data(vertexdata, GL_VERTEX_SHADER);
			if (!vshader)
				goto falied;
			glAttachShader(program, vshader);
		}

		if (geometrydata) {
			gshader = shader::create_shader_from_data(geometrydata, GL_GEOMETRY_SHADER);
			if (!gshader)
				goto falied;
			glAttachShader(program, gshader);
		}

		if (fragmentdata) {
			fshader = shader::create_shader_from_data(fragmentdata, GL_FRAGMENT_SHADER);
			if (!fshader)
				goto falied;
			glAttachShader(program, fshader);
		}

		if (!beforelinkFuntion(program,vshader, fshader, gshader))
			goto falied;;

		glLinkProgram(program);
		GLint link_ok = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			print_log(program);
			goto falied;
		}
		if (vshader) glDeleteShader(vshader);
		if (fshader) glDeleteShader(fshader);
		return program;

	falied:
		if (vshader) glDeleteShader(vshader);
		if (fshader) glDeleteShader(fshader);
		if (program)  glDeleteProgram(program);
		return -1;
	}


	MangerTemplateClassCPP(shadermanger)
	MangerTemplateClassCPP(rendershadermanger)
	MangerTemplateClassCPP(computershadermanger)
		
////////////////////////shader////////////////////////////////////////////////

	void shader::reset()
	{
		if (is_available())
		{
			glDeleteProgram(m_program);
			m_program = -1;
		}
	}

	bool shader::CheckValidate()
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "not available shader,log:program = -1";
			return false;
		}


		GLint logLength;
		glValidateProgram(m_program);
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			GLchar *log = (GLchar *)malloc(logLength);
			glGetProgramInfoLog(m_program, logLength, &logLength, log);
			BOOST_LOG_TRIVIAL(error) << "not available shader,log:" << log;
			free(log);
			return false;
		}

		return true;
	}



	bool extension_deal(std::string path, std::string& data)
	{
		//可以用\或者/表示路径，但是取最远的那一个
		int pathrootpos = max((int)path.find_last_of('\\'), (int)path.find_last_of('/'));
		//包含\或者/的主路径
		std::string rootpath = (pathrootpos != std::string::npos) ? path.substr(0, pathrootpos + 1) : "";
		std::string result;
		int pos;
		int begin = 0;
		data += '\n';//避免最后一行没有这个符号无法解析的尴尬
		while ((begin<data.size()
			&& (pos = data.find("\n", begin)) != std::string::npos))
		{
			std::string linestr = data.substr(begin, pos - begin);
			std::string lineresult = linestr;
			//扩展1 #include 就是把后面文件夹的内容加入到当前文件
#define  COMD_INCLUDE     "#include"
#define  COMD_IFNDEF    "#ifndef"
#define  COMD_DEFINE     "#define"
#define  COMD_ENDIF    "#endif"
			int definepos = linestr.find(COMD_INCLUDE);
			if (definepos != std::string::npos)
			{
				std::string filename = linestr.substr(definepos + strlen(COMD_INCLUDE), linestr.size() - definepos - strlen(COMD_INCLUDE));
				if (filename[filename.size() - 1] = '\t')
					filename = filename.substr(0, filename.size() - 1);
				StringStream::Trim(filename);
				StringStream::Trimc(filename, std::string("\""));
				if (filename.size() == 0) {
					BOOST_LOG_TRIVIAL(error) << "extension #include nonfile:" << linestr;
					return false;
				}
				if (rootpath.size() > 0)
					filename = rootpath + filename;

				if (!file_read(filename.data(), lineresult)) {
					BOOST_LOG_TRIVIAL(error) << "extension #include file load failed:" << filename;
					return false;
				}
				//递归，给附加的文件也同样的规则，这样带来的问题是过滤方法要考虑递归的情况，不能直接字符串
				//改变机制为如果这个行经过include处理了就不能进行其他的字符处理，这样就可以满足使用这个的初衷
				if (!extension_deal(filename, lineresult))
					return false;
				//goto next;

			}

			//这里添加其他的规则


			//现在可以识别"#ifndef"命令，所以不屏蔽
			//先跳过下面命令,
			//bug：如果包含的文件中有这些命令的注释将导致整个文件无法包含，这里只是暂时这样，暂不处理
			//int skippos = std::max((int)linestr.find(COMD_ENDIF),(int)std::max((int)linestr.find(COMD_IFNDEF), (int)linestr.find(COMD_DEFINE)));
			//if (skippos >= 0) {
			//	lineresult = "";
			//}
			//next:
			result += lineresult + "\r\n";
			begin = pos + 1;//跳过\n
		};

		//将剩下的数据补全,begin此时指向下一个未解析的数据,在开始的时候加上'\n'避免出现此种情况
		/*if (begin < data.size()) {
		result += data.substr(begin, data.size() - begin);
		}*/
		data = result;
		return true;
	}

	const char* SHADER_TYPE2STRING(GLUI::shader::SHADER_TYPE type)
	{
		switch (type)
		{
		case GLUI::shader::VERTEX:return "VERTEX"; break;
		case GLUI::shader::GEOMETRY:return "GEOMETRY"; break;
		case GLUI::shader::FRAGMENT:return "FRAGMENT"; break;
		case GLUI::shader::COMPUTE:return "COMPUTE"; break;
		case GLUI::shader::TESS_CONTROL:return "TESS_CONTROL"; break;
		case GLUI::shader::TESS_EVALUATION:return "TESS_EVALUATION"; break;
		default:break;
		}
		return "unknowtype";
	}

	bool shader::createshader(std::initializer_list<shader_creater_struct> shader_creater_structs, const std::function<bool(GLuint)> &beforelinkFuntion)
	{
		reset();
		bool ret = false;
		std::map<SHADER_TYPE, std::string> shaders;
		std::vector<GLuint> shaderIDs;
		GLuint program = 0;
		for (auto shadersinfo = shader_creater_structs.begin(); shadersinfo != shader_creater_structs.end(); shadersinfo++)
		{
			SHADER_TYPE type = shadersinfo->shadertype;
			//检测是否存在同类型，如果类型相同则错误
			if (shaders.find(type) != shaders.end())
			{
				BOOST_LOG_TRIVIAL(error) << "repeat: "
					                     << SHADER_TYPE2STRING(shadersinfo->shadertype)
					                     << ",check again!";
				goto enddeal;
			}
			//加载shader数据
			if (shadersinfo->isdata) 
				shaders[type] = shadersinfo->param;
			else 
			{
				if (!file_read(shadersinfo->param, shaders[type])) {
					BOOST_LOG_TRIVIAL(error) 
						<< "open file failed,filepath:" 
						<< shadersinfo->param;
					goto enddeal;
				}
				//附加扩展内容
				if (!extension_deal(shadersinfo->param, shaders[type]))
					goto enddeal;
			}
			
			BOOST_LOG_TRIVIAL(info) << "str size:" << shaders[type].size();
			BOOST_LOG_TRIVIAL(info) << SHADER_TYPE2STRING(shadersinfo->shadertype) 
				                    << shaders[type];	
		}
		program = glCreateProgram();
		if (program == 0) {
			BOOST_LOG_TRIVIAL(error) << "shader::createshader:  glCreateProgram failed";
			goto enddeal;
		}

		for (std::map<SHADER_TYPE, std::string>::iterator itr = shaders.begin();
			itr != shaders.end(); itr++)
		{
			GLuint shaderid = shader::create_shader_from_data(itr->second.data(), itr->first);
			if (!shaderid)
				goto enddeal;
			shaderIDs.push_back(shaderid);
			glAttachShader(program, shaderid);
			LAZY_OPENGL_DO("shader::createshader: glAttachShader", goto enddeal;)
		}
		 
		if (!beforelinkFuntion(program))
			goto enddeal;

		glLinkProgram(program);
		GLint link_ok = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		if (!link_ok) 
		{
			print_log(program);
			goto enddeal;
		}
		m_program = program;
		ret = true;
	enddeal:
		for (std::vector<GLuint>::iterator itr = shaderIDs.begin();
			 itr != shaderIDs.end(); itr++){
			glDeleteShader(*itr);
		}
		if (!ret&&program)  glDeleteProgram(program);
		return ret;

	}
////////////////////////////////////////////////////////////////////



	GLint shader::get_attrib(const char *name)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "getting attribute from not available shader";
			return -1;
		}

		GLint attribute = glGetAttribLocation(m_program, name);

		/*if (attribute == -1)
		BOOST_LOG_TRIVIAL(error) << "Could not bind attribute" << name
		<<"  opengl error str:"<< opengl_errorcode2string(glGetError());*/
		return attribute;
	}

	GLint shader::get_uniform(const char *name)
	{
		GLint uniform = glGetUniformLocation(m_program, name);
		LAZY_OPENGL_DO("shader::get_uniform : glGetUniformLocation failed", return -1;)
		return uniform;
	}

	GLuint shader::GetUniformBlockIndex(const char *blockname)
	{
	
		GLuint ret = glGetUniformBlockIndex(m_program, blockname);
		LAZY_OPENGL_DO("shader::GetUniformBlockIndex : glGetUniformBlockIndex failed", return -1;)
		return ret;
	}

	bool shader::bindUniformBlockIndexwithblockpoint(GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		glUniformBlockBinding(m_program, uniformBlockIndex, uniformBlockBinding);
		LAZY_OPENGL_RETURN_FALSE("shader::bindUniformBlockIndexwithblockpoint : glUniformBlockBinding failed")
		return true;
	}


	bool shader::set_uniform_block(const char *blockname, GLuint uniformBlockBinding)
	{
		GLuint index = GetUniformBlockIndex(blockname);
		if (index != -1)
			return bindUniformBlockIndexwithblockpoint(index, uniformBlockBinding);
		return false;
	}

#define MAX_NAME_SIZE 201

	void shader::get_program_all_attrib(std::vector<glsl_value>& list)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "getting all attribute from not available shader";
			return;
		}

		int nrAttributes;
		glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
		char buffer[MAX_NAME_SIZE] = { 0 };
		int maxwritesize = sizeof(buffer) - 1;
		int realwritesize = 0;
		list.clear();
		for (int i = 0; i < nrAttributes; ++i)
		{
			glsl_value value;
			glGetActiveAttrib(m_program, i, maxwritesize, &realwritesize, &value.size, &value.glsl_value_type, buffer);
			value.name = std::string(buffer);
			value.location = get_attrib(buffer);
			list.push_back(value);
		}
	}


	void shader::get_program_all_uniform(std::vector<glsl_value>& list)
	{
		int nrUniforms;
		glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &nrUniforms);
		char buffer[MAX_NAME_SIZE] = { 0 };
		int maxwritesize = sizeof(buffer) - 1;
		int realwritesize = 0;
		list.clear();
		for (int i = 0; i < nrUniforms; ++i)
		{
			glsl_value value;
			glGetActiveAttrib(m_program, i, maxwritesize, &realwritesize, &value.size, &value.glsl_value_type, buffer);
			value.name = std::string(buffer);
			value.location = get_attrib(buffer);
			list.push_back(value);
		}
	}




	void shader::set_uniform(const char* valuename, glslvalue& value)
	{
		switch (value.type())
		{
		case glslvalue::GLSL_INT: { set_uniform_int(valuename, value.As_int()); }break;
		case glslvalue::GLSL_INT_VEC2: { set_uniform_int_vec2_array(valuename, 1, static_cast<GLint*>(&value.As_ivec2()[0])); }break;
		case glslvalue::GLSL_INT_VEC3: { set_uniform_int_vec3_array(valuename, 1, static_cast<GLint*>(&value.As_ivec3()[0])); }break;
		case glslvalue::GLSL_INT_VEC4: { set_uniform_int_vec4_array(valuename, 1, static_cast<GLint*>(&value.As_ivec4()[0])); }break;
		case glslvalue::GLSL_INT_VEC2_ARRAY: { set_uniform_int_vec2_array(valuename, value.get_intvec2array_size(), (GLint*)value.As_intvec2array()); }break;
		case glslvalue::GLSL_INT_VEC3_ARRAY: { set_uniform_int_vec3_array(valuename, value.get_intvec3array_size(), (GLint*)value.As_intvec3array()); }break;
		case glslvalue::GLSL_INT_VEC4_ARRAY: { set_uniform_int_vec4_array(valuename, value.get_intvec4array_size(), (GLint*)value.As_intvec4array()); }break;

		case glslvalue::GLSL_FLOAT: { set_uniform_float(valuename, value.As_float()); }break;
		case glslvalue::GLSL_FLOAT_VEC2: { set_uniform_float_vec2_array(valuename, 1, static_cast<GLfloat*>(&value.As_fvec2()[0])); }break;
		case glslvalue::GLSL_FLOAT_VEC3: { set_uniform_float_vec3_array(valuename, 1, static_cast<GLfloat*>(&value.As_fvec3()[0])); }break;
		case glslvalue::GLSL_FLOAT_VEC4: { set_uniform_float_vec4_array(valuename, 1, static_cast<GLfloat*>(&value.As_fvec4()[0])); }break;
		case glslvalue::GLSL_FLOAT_VEC2_ARRAY: { set_uniform_float_vec2_array(valuename, value.get_intvec2array_size(), (GLfloat*)value.As_intvec2array()); }break;
		case glslvalue::GLSL_FLOAT_VEC3_ARRAY: { set_uniform_float_vec3_array(valuename, value.get_intvec3array_size(), (GLfloat*)value.As_intvec3array()); }break;
		case glslvalue::GLSL_FLOAT_VEC4_ARRAY: { set_uniform_float_vec4_array(valuename, value.get_intvec4array_size(), (GLfloat*)value.As_intvec4array()); }break;

		case glslvalue::GLSL_FLOAT_MAT2: { set_uniform_mat_2_2(valuename, (GLfloat*)&value.As_fmat2()[0][0]); }break;
		case glslvalue::GLSL_FLOAT_MAT3: { set_uniform_mat_3_3(valuename, (GLfloat*)&value.As_fmat3()[0][0]); }break;
		case glslvalue::GLSL_FLOAT_MAT4: { set_uniform_mat_4_4(valuename, (GLfloat*)&value.As_fmat4()[0][0]); }break;

		case glslvalue::GLSL_FLOAT_MAT2_ARRAY: { set_uniform_mat_2_2_array(valuename, value.get_floatmat2array_size(), (GLfloat*)value.As_floatmat2array()); }break;
		case glslvalue::GLSL_FLOAT_MAT3_ARRAY: { set_uniform_mat_3_3_array(valuename, value.get_floatmat3array_size(), (GLfloat*)value.As_floatmat3array()); }break;
		case glslvalue::GLSL_FLOAT_MAT4_ARRAY: { set_uniform_mat_4_4_array(valuename, value.get_floatmat4array_size(), (GLfloat*)value.As_floatmat4array()); }break;
		default: {	BOOST_LOG_TRIVIAL(warning) << "shaderhelper::setshader: glslvalue is empty "; }break;
		}
	}

	////////1
	void shader::set_uniform_int(const char* valuename, GLint value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform1i(locid, value);
	}


	void shader::set_uniform_int_array(const char* valuename, GLsizei count, GLint* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform1iv(locid, count, value);
	}

	void shader::set_uniform_float(const char* valuename, GLfloat value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform1f(locid, value);
	}


	void shader::set_uniform_Float_array(const char* valuename, GLsizei count, GLfloat* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform1fv(locid, count, value);
	}

	/////////2
	void shader::set_uniform_int_vec2(const char* valuename, GLint value1, GLint value2)
	{
		int locid = get_attrib(valuename);
		if (locid >= 0)
			glUniform2i(locid, value1, value2);
	}

	void shader::set_uniform_int_vec2_array(const char* valuename, GLsizei count, GLint* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform2iv(locid, count, value);
	}


	void shader::set_uniform_float_vec2(const char* valuename, GLfloat value1, GLfloat value2)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform2f(locid, value1, value2);
	}

	void shader::set_uniform_float_vec2_array(const char* valuename, GLsizei count, GLfloat* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform2fv(locid, count, value);
	}

	///////3

	void shader::set_uniform_int_vec3(const char* valuename, GLint value1, GLint value2, GLint value3)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform3i(locid, value1, value2, value3);
	}

	void shader::set_uniform_int_vec3_array(const char* valuename, GLsizei count, GLint* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform3iv(locid, count, value);
	}


	void shader::set_uniform_float_vec3(const char* valuename, GLfloat value1, GLfloat value2, GLfloat value3)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform3f(locid, value1, value2, value3);
	}

	void shader::set_uniform_float_vec3_array(const char* valuename, GLsizei count, GLfloat* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform3fv(locid, count, value);
	}

	//////4

	void shader::set_uniform_int_vec4(const char* valuename, GLint value1, GLint value2, GLint value3, GLint value4)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform4i(locid, value1, value2, value3, value4);
	}

	void shader::set_uniform_int_vec4_array(const char* valuename, GLsizei count, GLint* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform4iv(locid, count, value);
	}


	void shader::set_uniform_float_vec4(const char* valuename, GLfloat value1, GLfloat value2, GLfloat value3, GLfloat value4)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform4f(locid, value1, value2, value3, value4);
	}

	void shader::set_uniform_float_vec4_array(const char* valuename, GLsizei count, GLfloat* value)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniform4fv(locid, count, value);
	}

	void shader::set_uniform_mat_2_2(const char* valuename, GLfloat* value, GLboolean tran)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniformMatrix2fv(locid, 1, tran, value);
	}

	void shader::set_uniform_mat_3_3(const char* valuename, GLfloat* value, GLboolean tran)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniformMatrix3fv(locid, 1, tran, value);
	}


	void shader::set_uniform_mat_4_4(const char* valuename, GLfloat* value, GLboolean tran)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniformMatrix4fv(locid, 1, tran, value);
	}

	void shader::set_uniform_mat_2_2_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniformMatrix2fv(locid, count, tran, value);
	}

	void shader::set_uniform_mat_3_3_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniformMatrix3fv(locid, count, tran, value);
	}

	void shader::set_uniform_mat_4_4_array(const char* valuename, GLsizei count, GLfloat* value, GLboolean tran)
	{
		int locid = get_uniform(valuename);
		if (locid >= 0)
			glUniformMatrix4fv(locid, count, tran, value);
	}


	bool shader::set_subroutine(SHADER_TYPE shadertype, std::map<std::string, std::string>& funnames)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "set_subroutine from not available shader";
			return false;
		}

		//这边获得全部的子成员函数，如果有任何一个子函数并没有赋值，那么就会在opengl里面产生一个错误，所以这里务必把全部包含进来
		if (get_uniform_subroutine_num(shadertype) != funnames.size()){
			BOOST_LOG_TRIVIAL(error) << "必须为所有的子函数定义实现方法";
			return false;
		}

		std::vector<GLuint> indexs;
		indexs.resize(funnames.size());
		for (auto funname = funnames.begin(); funname != funnames.end(); funname++)
		{
			GLint templateId = get_subroutine_uniform(shadertype, funname->first.data());
			if (templateId == -1)
				return false;

			GLuint Index = get_subroutine_index(shadertype, funname->second.data());
			if (Index == -1)
				return false;
			indexs[templateId]=Index;
		}
		
		glUniformSubroutinesuiv(shadertype, indexs.size(), &indexs[0]);

		return opengl_error("glUniformSubroutinesuiv failed");
	}


	/*bool shader::set_subroutine(SHADER_TYPE shadertype, std::initializer_list<std::string> funnames)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "set_subroutine from not available shader";
			return false;
		}

		GLint templateId = get_subroutine_uniform(shadertype, templatename);
		if (templateId == -1) 
			return false;
		

		GLuint Index = get_subroutine_index(shadertype, funname);
		if (Index == -1)
			return false;

		return set_uniform_subroutine_index(shadertype,Index);
	}*/

	bool shader::get_active_subroutine_uniform_name(SHADER_TYPE shadertype, GLuint Index, std::string& result)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "set_subroutine from not available shader";
			return false;
		}
		GLint length;
		glGetActiveSubroutineUniformiv(m_program, shadertype, Index, GL_UNIFORM_NAME_LENGTH, &length);
		if (!opengl_error("get_active_subroutine_uniform_name: glGetActiveSubroutineUniformiv failed"))
			return false;

		result.resize(length);

		//经验证，这个返回的字符串结尾自带'0'结束符，所以不用特意加0处理
		glGetActiveSubroutineUniformName(m_program, shadertype, Index, length, nullptr, &result[0]);
		if (!opengl_error("get_active_subroutine_uniform_name: glGetActiveSubroutineUniformName failed"))
			return false;

		return true;
	}

/////////////rendershader////////////////////////////////////////////

	void rendershader::create_program_from_data(const char* vertexdata, const char* geometrydata, const char *fragmentdata,const std::function<bool(GLuint, GLuint, GLuint,GLuint)> &beforelinkFuntion)
	{
		reset();
		m_program = create_program_with_fun(vertexdata, geometrydata, fragmentdata, beforelinkFuntion);
	}
	void rendershader::create_program_from_file(const char* vertexpath, const char *geometrypath, const char *fragmentpath, const std::function<bool(GLuint, GLuint, GLuint, GLuint)> &beforelinkFuntion)
	{
		reset();

		GLchar* vertexsource = nullptr, *fragmentsource = nullptr, *geometrysource = nullptr;

		if (vertexpath) {
			vertexsource = file_read(vertexpath);
			if (vertexsource == NULL) {
				BOOST_LOG_TRIVIAL(error) << "open  vertex shader file failed,filepath:" << vertexpath;
				return;
			}
		}
		auto safedelete_vertexsource = GLUI::MakeGuard(vertexsource, free);//no matter what ,delete

		if (geometrypath) {
			geometrysource = file_read(geometrypath);
			if (geometrysource == NULL) {
				BOOST_LOG_TRIVIAL(error) << "open  fragment shader file failed,filepath:" << geometrypath;
				return;
			}
		}
		auto safedelete_geometrysource = GLUI::MakeGuard(geometrysource, free);//no matter what ,delete

		if (fragmentpath) {
			fragmentsource = file_read(fragmentpath);
			if (fragmentsource == NULL) {
				BOOST_LOG_TRIVIAL(error) << "open  fragment shader file failed,filepath:" << fragmentpath;
				return;
			}
		}
		auto safedelete_fragmentsource = GLUI::MakeGuard(fragmentsource, free);//no matter what ,delete

		m_program = create_program_with_fun(vertexsource, geometrysource, fragmentsource, beforelinkFuntion);
	}

	


	void rendershader::create_program_from_file_extension(const char* vertexpath, const char *geometrypath, const char *fragmentpath, const std::function<bool(GLuint, GLuint, GLuint, GLuint)> &beforelinkFuntion)
	{
		reset();
		std::string vertexsource, fragmentsource, geometrysource;
		if (vertexpath) {
			if (!file_read(vertexpath, vertexsource)) {
				BOOST_LOG_TRIVIAL(error) << "open  vertex shader file failed,filepath:" << vertexpath;
				return;
			}
			if (!extension_deal(vertexpath, vertexsource)) {
				return;
			}

			BOOST_LOG_TRIVIAL(error) << vertexsource;
		}

		if (geometrypath) {
			if (!file_read(geometrypath, geometrysource)) {
				BOOST_LOG_TRIVIAL(error) << "open  geometry shader file failed,filepath:" << geometrypath;
				return;
			}
		}

		if (fragmentpath) {
			if (!file_read(fragmentpath, fragmentsource)) {
				BOOST_LOG_TRIVIAL(error) << "open  fragment shader file failed,filepath:" << fragmentpath;
				return;
			}
			if (!extension_deal(fragmentpath, fragmentsource)) {
				return;
			}
			BOOST_LOG_TRIVIAL(error) << fragmentsource;
		}
		m_program = create_program_with_fun(vertexpath ? vertexsource.data() : nullptr, geometrypath ? geometrysource.data() : nullptr, fragmentpath ? fragmentsource.data() : nullptr, beforelinkFuntion);

	}


#else
	GLuint create_program(const char* vertexdata, const char *fragmentdata)
	{
		GLuint program = glCreateProgram();
		GLuint vshader = -1, fshader = -1;
		if (vertexdata) {
			vshader = create_shader_from_data(vertexdata, GL_VERTEX_SHADER);
			if (!vshader)
				goto falied;
			glAttachShader(program, vshader);
		}
		if (fragmentdata) {
			fshader = create_shader_from_data(fragmentdata, GL_FRAGMENT_SHADER);
			if (!fshader)
				goto falied;
			glAttachShader(program, fshader);
		}
		glLinkProgram(program);
		GLint link_ok = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			print_log(program);
			goto falied;
		}
		if (vshader) glDeleteShader(vshader);
		if (fshader) glDeleteShader(fshader);
		return program;

	falied:
		if (vshader) glDeleteShader(vshader);
		if (fshader) glDeleteShader(fshader);
		if (program)  glDeleteProgram(program);
		return -1;
	}
	void rendershader::create_program_from_file(const char* vertexpath, const char *fragmentpath)
	{
		reset();

		GLchar* vertexsource = nullptr, *fragmentsource = nullptr;

		if (vertexpath) {
			vertexsource = file_read(vertexpath);
			if (vertexsource == NULL) {
				BOOST_LOG_TRIVIAL(error) << "open  vertex shader file failed,filepath:" << vertexpath;
				return;
			}
		}
		auto safedelete_vertexsource = GLUI::MakeGuard(vertexsource, free);//no matter what ,delete

		if (fragmentpath) {
			fragmentsource = file_read(fragmentpath);
			if (fragmentsource == NULL) {
				BOOST_LOG_TRIVIAL(error) << "open  fragment shader file failed,filepath:" << fragmentpath;
				return;
			}
		}
		auto safedelete_fragmentsource = GLUI::MakeGuard(fragmentsource, free);//no matter what ,delete

		m_program = create_program(vertexsource, fragmentsource);
	}


	void rendershader::create_program_from_data(const char* vertexdata, const char *fragmentdata)
	{
		reset();
		m_program = create_program(vertexdata, fragmentdata);
	}
#endif // USE_GEOMETRY_SHADER







	///////////////////////computershader/////////////////////////////////////////////////////
	void computershader::create_compute_program_from_data(const char* computedata)
	{
		GLuint program = glCreateProgram();
		GLuint vcompute = -1;
		vcompute = create_shader_from_data(computedata, GL_COMPUTE_SHADER);
		if (!vcompute)
			return;
		glAttachShader(program, vcompute);
		glLinkProgram(program);
		GLint link_ok = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			print_log(program);
			return;
		}
		glDeleteShader(vcompute);
		m_program = program;
	}
	void computershader::create_compute_program_from_file(const char* computepath)
	{
		reset();
		GLchar* source = nullptr;
		source = file_read(computepath);
		auto safedelete_source = GLUI::MakeGuard(source, free);//no matter what ,delete
		if (source == NULL) {
			BOOST_LOG_TRIVIAL(error) << "open  compute shader file failed,filepath:" << computepath;
			return;
		}
		return create_compute_program_from_data(source);
	}

	void computershader::create_compute_program_from_file_extension(const char* computepath)
	{
		reset();
		std::string computesource;
		if (!file_read(computepath, computesource)) {
				BOOST_LOG_TRIVIAL(error) << "open  compute shader file failed,filepath:" << computepath;
				return;
		}
		if (!extension_deal(computepath, computesource)) {
			return;
		}
		BOOST_LOG_TRIVIAL(error) << computesource;
		return create_compute_program_from_data(computesource.data());
	}

	bool computershader::get_max_compute_work_group_size(int& x, int& y, int& z)
	{
		int param[3];
		glGetProgramiv(m_program, GL_MAX_COMPUTE_WORK_GROUP_SIZE, param);
		if (!opengl_error("get_max_compute_work_group_size"))
			return false;
		x = param[0];
		y = param[1];
		z = param[2];
		return true;
	}

}