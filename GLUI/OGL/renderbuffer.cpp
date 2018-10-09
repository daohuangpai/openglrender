#include "renderbuffer.h"

namespace GLUI
{

	bool renderbuffer::generate(GLenum internalformat​, GLsizei width​, GLsizei height, GLsizei samples​)
	{
		m_generatesuccess = false;

		if (!is_available())
			glGenRenderbuffers(1, &m_renderbufferid);

		//采样数不能超出边界，
		if (samples​ != 0)
		{
			//测试是否可以使用
			GLint maxbuffers = 0;
			glGetIntegerv(GL_MAX_SAMPLES, &maxbuffers);
			if (samples​ > maxbuffers) {
				BOOST_LOG_TRIVIAL(error) << "renderbuffer::generate: too much sample is not support ,current:" << samples​
					<< " Max is " << maxbuffers;
				return false;
			}
		}

		//glRenderbufferStorage is equivalent to calling glRenderbufferStorageMultisample with the samples​ set to zero.，所以只要这个就够了
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples​, internalformat​,width​, height);
		LAZY_OPENGL_RETURN_FALSE("renderbuffer::generate: glRenderbufferStorageMultisample failed");
		m_generatesuccess = true;
		return true;
	}



}