#include "feedback.h"
#include "openglerror.h"
#include "lazytool.h"

namespace GLUI
{
	transformfeedback::~transformfeedback()
	{
		if (is_available())
			glDeleteTransformFeedbacks(1, &m_transformfeedbackid);
		opengl_error("transformfeedback::~transformfeedback: glDeleteTransformFeedbacks failed");
	}

	bool transformfeedback::generate()
	{
		if (is_available())
			return true;
		glCreateTransformFeedbacks(1,&m_transformfeedbackid);
		LAZY_OPENGL_RETURN_FALSE("transformfeedback::generate: glCreateTransformFeedbacks failed")
		return true;
	}

	bool transformfeedback::Drow(Vao* vao, Vao::DROW_TYPE drowtype, int databegin, int data_size)
	{
		if (!bind())
			return false;
		glBeginTransformFeedback(drowtype);
		LAZY_OPENGL_RETURN_FALSE("transformfeedback::run: glBeginTransformFeedback failed")
		bool ret = vao->DrowArray(drowtype, databegin, data_size);
		glEndTransformFeedback();
		LAZY_OPENGL_RETURN_FALSE("transformfeedback::run: glEndTransformFeedback failed")
		return ret;
	}


	bool transformfeedback::init(GLuint program, BUFFER_OUT_TYPE outtype, char** outname, int outnamesize)
	{
		if (!generate())
			return false;
		if (!bind())
			return false;
		glTransformFeedbackVaryings(program, outnamesize, outname, outtype);
		LAZY_OPENGL_RETURN_FALSE("transformfeedback::init: glTransformFeedbackVaryings failed")
		return true;
	}

	bool transformfeedback::PauseTransformFeedback()
	{
		if (!bind())
			return false;
		glPauseTransformFeedback();
		LAZY_OPENGL_RETURN_FALSE("transformfeedback::PauseTransformFeedback: glPauseTransformFeedback failed")
		return true;
	}

	bool transformfeedback::ResumeTransformFeedback()
	{
		if (!bind())
			return false;
		glResumeTransformFeedback();
		LAZY_OPENGL_RETURN_FALSE("transformfeedback::ResumeTransformFeedback: glResumeTransformFeedback failed")
		return true;
	}
	

	////使用输出绑定到outname的数组，每一个元素对应一个
	//bool transformfeedback::init(char* vectex, char* fragment, char** outname, int outnamesize, BUFFERTYPE type,const std::function<bool(void)> &initVao)
	//{
	//
	//	m_feekbackshader.create_program_from_file_extension(vectex, fragment, nullptr, 
	//		[&](GLuint program ,GLuint v , GLuint g, GLuint f)->bool
	//	  {
	//		glTransformFeedbackVaryings(program, outnamesize, outname, type);
	//		LAZY_OPENGL_RETURN_FALSE("transformfeedback::init: glTransformFeedbackVaryings failed")
	//		return true;
	//	  }
	//	);

	//	if (!m_feekbackshader.is_available())
	//		return false;

	//	if (!initVao())
	//		return false;

	//	return true;
	//}

	//bool transformfeedback::run(Vao::DROW_TYPE drowtype, int databegin, int data_size)
	//{
	//   //使能禁止光栅化，就是不画图
	//	glEnable(GL_RASTERIZER_DISCARD);
	//	LAZY_OPENGL_RETURN_FALSE("transformfeedback::run: glEnable GL_RASTERIZER_DISCARD failed")
	//	glBeginTransformFeedback(drowtype);
	//	LAZY_OPENGL_RETURN_FALSE("transformfeedback::run: glBeginTransformFeedback failed")
	//	m_vao.DrowArray(drowtype, databegin, data_size);
	//	glEndTransformFeedback();
	//	LAZY_OPENGL_RETURN_FALSE("transformfeedback::run: glEndTransformFeedback failed")
	//	glDisable(GL_RASTERIZER_DISCARD);
	//	LAZY_OPENGL_RETURN_FALSE("transformfeedback::run: glDisable GL_RASTERIZER_DISCARD failed")
	//	//保证渲染完成
	//	glFlush();
	//	return true;
	//}
	//
}