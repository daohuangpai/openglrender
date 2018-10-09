#include "openglerror.h"
#include <boost/log/trivial.hpp>

char* opengl_errorcode2string(GLenum code)
{
	char* result =nullptr;
	switch(code)
	{
		case GL_NO_ERROR:
			result =  "GL_NO_ERROR:当前无错误值";
			break;
		case GL_INVALID_ENUM:
			result = "GL_INVALID_ENUM:仅当使用非法枚举参数时，如果使用该参数有指定环境，则返回 GL_INVALID_OPERATION";
			break;
		case GL_INVALID_VALUE:
			result = "GL_INVALID_VALUE:仅当使用非法枚举参数时，如果使用该参数有指定环境，则返回 GL_INVALID_OPERATION";
			break;
		case GL_INVALID_OPERATION:
			result = "GL_INVALID_OPERATION:命令的状态集合对于指定的参数非法。";
			break;
		case GL_STACK_OVERFLOW:
			result = "GL_STACK_OVERFLOW:压栈操作超出堆栈大小。";
			break;
		case GL_STACK_UNDERFLOW:
			result = "GL_STACK_UNDERFLOW:出栈操作达到堆栈底部。";
			break;
		case GL_OUT_OF_MEMORY:
			result = "GL_OUT_OF_MEMORY:不能分配足够内存时。";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			result = "GL_INVALID_FRAMEBUFFER_OPERATION:当操作未准备好的真缓存时。";
			break;
		case GL_CONTEXT_LOST:
			result = "GL_CONTEXT_LOST:由于显卡重置导致 OpenGL context 丢失。";
			break;
		default:
			result = "未知错误";
			break;
	}
	return result;
 
}

 