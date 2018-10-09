#include "openglerror.h"
#include <boost/log/trivial.hpp>

char* opengl_errorcode2string(GLenum code)
{
	char* result =nullptr;
	switch(code)
	{
		case GL_NO_ERROR:
			result =  "GL_NO_ERROR:��ǰ�޴���ֵ";
			break;
		case GL_INVALID_ENUM:
			result = "GL_INVALID_ENUM:����ʹ�÷Ƿ�ö�ٲ���ʱ�����ʹ�øò�����ָ���������򷵻� GL_INVALID_OPERATION";
			break;
		case GL_INVALID_VALUE:
			result = "GL_INVALID_VALUE:����ʹ�÷Ƿ�ö�ٲ���ʱ�����ʹ�øò�����ָ���������򷵻� GL_INVALID_OPERATION";
			break;
		case GL_INVALID_OPERATION:
			result = "GL_INVALID_OPERATION:�����״̬���϶���ָ���Ĳ����Ƿ���";
			break;
		case GL_STACK_OVERFLOW:
			result = "GL_STACK_OVERFLOW:ѹջ����������ջ��С��";
			break;
		case GL_STACK_UNDERFLOW:
			result = "GL_STACK_UNDERFLOW:��ջ�����ﵽ��ջ�ײ���";
			break;
		case GL_OUT_OF_MEMORY:
			result = "GL_OUT_OF_MEMORY:���ܷ����㹻�ڴ�ʱ��";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			result = "GL_INVALID_FRAMEBUFFER_OPERATION:������δ׼���õ��滺��ʱ��";
			break;
		case GL_CONTEXT_LOST:
			result = "GL_CONTEXT_LOST:�����Կ����õ��� OpenGL context ��ʧ��";
			break;
		default:
			result = "δ֪����";
			break;
	}
	return result;
 
}

 