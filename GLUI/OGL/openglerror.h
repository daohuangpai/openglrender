#pragma once

#include <GL/glew.h>
#include <boost/log/trivial.hpp>

//关于具体错在哪里在ogl wiki可以查，比如
//https://www.khronos.org/opengl/wiki/GLAPI/glBindFramebuffer最底部有一个
//Errors
//GL_INVALID_ENUM is generated if target​ is not GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER.
//GL_INVALID_OPERATION is generated if framebuffer​ is not zero or the name of a framebuffer previously returned from a call to glGenFramebuffers.
//不写入这个内容，不然太麻烦
char* opengl_errorcode2string(GLenum code);
inline bool opengl_error(const char* info)
{
	int code = glGetError();
	bool ret;
	if (code != GL_NO_ERROR) {
		BOOST_LOG_TRIVIAL(error) << info << "   opengl error:" << opengl_errorcode2string(code) << " code id:" << code;
		ret = false; 
	}
	else ret = true; 
	return ret;
}

#define LAZY_OPENGL_DO(REPORT,DO)\
     if (!opengl_error(REPORT)) {\
	    DO\
     }

#define LAZY_OPENGL_RETURN_FALSE(REPORT)\
    LAZY_OPENGL_DO(REPORT,return false;)


#ifdef _DEBUG
#define LOG_OPEGNGL_ERROR(device,ret,ERRORINFO) \
         {                     \
         int LOG_OPEGNGL_ERROR_code = glGetError();\
         if (LOG_OPEGNGL_ERROR_code!=GL_NO_ERROR ){\
                device << ERRORINFO<<"opengl error:" << opengl_errorcode2string(LOG_OPEGNGL_ERROR_code)<< " code id:" << LOG_OPEGNGL_ERROR_code; \
               ret = false;\
		  }\
		  else {\
					ret = true;\
		  }\
         }

#else
#define LOG_OPEGNGL_ERROR(device) 
#endif // DEBUG


