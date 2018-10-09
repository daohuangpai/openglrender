#include "buffer.h"
#include "openglerror.h"

namespace GLUI
{


	MangerTemplateClassCPP(buffermanger)

		//params? returns the size of the buffer object, measured in bytes. The initial value is 0.
	GLint buffer::query_buffersize()
	{
		bind();
		GLint result = -1;
		glGetBufferParameteriv(m_target, GL_BUFFER_SIZE, &result);
		if (result == -1) {
			BOOST_LOG_TRIVIAL(error) << "query_buffersize error!";
		}
		return result;
	}

	//params? returns the buffer object's usage pattern. The initial value is GL_STATIC_DRAW.
	GLint buffer::query_usage()
	{
		bind();
		GLint result = -1;
		glGetBufferParameteriv(m_target, GL_BUFFER_USAGE, &result);
		if (result == -1) {
			BOOST_LOG_TRIVIAL(error) << "query_usage error!";
		}
		return result;
	}

	//params? returns the access policy set while mapping the buffer object. The initial value is GL_READ_WRITE.
	GLint buffer::query_access()
	{
		bind();
		GLint result = -1;
		glGetBufferParameteriv(m_target, GL_BUFFER_ACCESS, &result);
		if (result == -1) {
			BOOST_LOG_TRIVIAL(error) << "query_access error!";
		}
		return result;
	}


	//params? returns a flag indicating whether the buffer object is currently mapped. The initial value is GL_FALSE.
	GLboolean buffer::query_mapped()
	{
		bind();
		GLint result = GL_FALSE;
		glGetBufferParameteriv(m_target, GL_BUFFER_MAPPED, &result);
		if (result == GL_FALSE) {
			BOOST_LOG_TRIVIAL(error) << "query_mapped error!";
		}
		return result;
	}

	bool buffer::ensuresize(int bytesize, bool tight)
	{
		if (bytesize > m_element_size)
		{
			int oldsize = m_element_size;
			std::vector<char> cpubuffer;
			cpubuffer.resize(oldsize);
			if (!CopyOutDataHuge(&cpubuffer[0], 0, oldsize))
				return false;
			int newsize = 0;

			if (tight)
			{
				newsize = bytesize;
			}			
			else
			{
				newsize = oldsize;
				int maxoffsetsize = 32;
				do {
					newsize = newsize << 1;
				} while ((newsize < bytesize) && (--maxoffsetsize > 0));

				if (maxoffsetsize < 0) {
					BOOST_LOG_TRIVIAL(error) << "buffer::ensuresize: size to big ,cannot get:" << bytesize;
					return false;
				}
			}
			
			bind();
			glBufferData(m_target, newsize, NULL, m_usage);
			LAZY_OPENGL_RETURN_FALSE("buffer::ensuresize: glBufferData failed")
			return UpdateDataHuge(&cpubuffer[0], 0, oldsize);
		}

		return true;
	}

	bool buffer::generate(char* data, int size, USAGE_MODE usagemode, TargetMode target)
	{
		m_target = target;
		m_usage = usagemode;
		if (!is_available())
			glGenBuffers(1, &m_bufferid);
		LAZY_OPENGL_RETURN_FALSE("buffer::generate: glGenBuffers failed")
		m_element_size = size;
		bind();
		LAZY_OPENGL_RETURN_FALSE("buffer::generate: bind() failed")
		glBufferData(m_target, m_element_size , data, m_usage);
		LAZY_OPENGL_RETURN_FALSE("buffer::generate: glBufferData failed")
		return true;
	}


	bool buffer::UpdateDataHuge(char* data, int offset, int size)
	{
		if (offset + size > m_element_size) {
			BOOST_LOG_TRIVIAL(error) << "data size out of buffer size!";
			return false;
		}

		bind();
		char* ptr = (char*)glMapBuffer(m_target, GL_WRITE_ONLY);
		LAZY_OPENGL_RETURN_FALSE("buffer::UpdateDataHuge: glMapBuffer failed")
		memcpy((ptr+offset), data, size);
		glUnmapBuffer(m_target);
		Unbind();
		return true;
	}


	bool buffer::CopyOutDataHuge(char* data, int offset, int size)
	{
		if (offset + size > m_element_size) {
			BOOST_LOG_TRIVIAL(error) << "data size out of buffer size!";
			return false;
		}

		bind();
		char* ptr = (char*)glMapBuffer(m_target, GL_READ_ONLY);
		LAZY_OPENGL_RETURN_FALSE("buffer::CopyOutDataHuge: glMapBuffer failed")
		memcpy(data, (ptr + offset), size);
		glUnmapBuffer(m_target);
		Unbind();
		return true;
	}


	bool buffer::UpdateDataNotHuge(char* data, int offset, int size)
	{
		if (offset + size > m_element_size) {
			BOOST_LOG_TRIVIAL(error) << "data size out of buffer size!";
			return false;
		}

		bind();
		char* ptr = (char*)glMapBufferRange(m_target, offset, size, GL_MAP_WRITE_BIT |GL_MAP_INVALIDATE_BUFFER_BIT);
		LAZY_OPENGL_RETURN_FALSE("buffer::UpdateDataNotHuge: glMapBufferRange failed")
		memcpy(ptr, data, size);
		glUnmapBuffer(m_target);
		Unbind();
		return true;
	}

	bool buffer::CopyOutDataNotHuge(char* data, int offset, int size)
	{
		if (offset + size > m_element_size) {
			BOOST_LOG_TRIVIAL(error) << "data size out of buffer size!";
			return false;
		}

		bind();
		char* ptr = (char*)glMapBufferRange(m_target, offset, size, GL_MAP_READ_BIT);
		LAZY_OPENGL_RETURN_FALSE("buffer::CopyOutDataNotHuge: glMapBufferRange failed")
		memcpy(data, ptr,size);
		glUnmapBuffer(m_target);
		Unbind();
		return true;
	}

	bool buffer::AccessData(AccessFun fun, void* param)
	{
		bind();
		char* ptr = (char*)glMapBuffer(m_target, GL_READ_WRITE);
		LAZY_OPENGL_RETURN_FALSE("buffer::AccessData: glMapBuffer failed")
		fun(ptr, m_element_size, param);
		glUnmapBuffer(m_target);
		Unbind();
		return true;
	}


	bool buffer::AccessData(const std::function<void(char*, int)> &AccessFuntion)
	{
		bind();
		char* ptr = (char*)glMapBuffer(m_target, GL_READ_WRITE);
		LAZY_OPENGL_RETURN_FALSE("buffer::AccessData: glMapBuffer failed")
		AccessFuntion(ptr, m_element_size);
		glUnmapBuffer(m_target);
		Unbind();
		return true;
	}

	bool buffer::BindToBinding(TargetMode mode, int bindingnum)
	{
		bind();
		switch (mode) {
		case ATOMIC_COUNTER_BUFFER:break;
		case TRANSFORM_FEEDBACK_BUFFER:break;
		case UNIFORM_BUFFER:break;
		case SHADER_STORAGE_BUFFER:break;
		default:
			BOOST_LOG_TRIVIAL(error) << " BindToBinding just support GL_ATOMIC_COUNTER_BUFFER,GL_TRANSFORM_FEEDBACK_BUFFER,GL_UNIFORM_BUFFER,GL_SHADER_STORAGE_BUFFER";
			return false;
		}
		glBindBufferBase(mode, bindingnum, m_bufferid);
		return opengl_error("BindToBinding");
	}

	bool buffer::BindBufferRange(TargetMode mode, int bindingnum, int beginindex, int size)
	{
		bind();
		switch (mode) {
		case ATOMIC_COUNTER_BUFFER:break;
		case TRANSFORM_FEEDBACK_BUFFER:break;
		case UNIFORM_BUFFER:break;
		case SHADER_STORAGE_BUFFER:break;
		default:
			BOOST_LOG_TRIVIAL(error) << " BindToBinding just support GL_ATOMIC_COUNTER_BUFFER,GL_TRANSFORM_FEEDBACK_BUFFER,GL_UNIFORM_BUFFER,GL_SHADER_STORAGE_BUFFER";
			return false;
		}
		glBindBufferRange(mode, bindingnum, m_bufferid,beginindex, size);
		return opengl_error("buffer::BindBufferRange");
	}

	bool buffer::copyfrombuffer(buffer* buffer, int srcoffset, int dstoffset, int size)
	{
		if (size - dstoffset > getsize()- srcoffset)
		{
			BOOST_LOG_TRIVIAL(error) << "copy data out of memory";
			return false;
		}

		char* dst = mappingwriteRange(srcoffset, size);
		if (!dst) return false;
		char* src = buffer->mappingreadRange(dstoffset, size);
		if (!src) return false;
		memcpy(dst, src, size);
		buffer->Unmapping();
		Unmapping();
		return true;
	}

}












/*
struct Division
{
Division(long Offset, long Size, GLenum Type, GLboolean Normalization, std::string Name)
{
offset = Offset;
size = Size;
type = Type;
normalization = Normalization;
name = Name;
}
long offset;//偏移位置
long size;//数据的个数，单位不一定是byte，随类型type的改变而改变
GLenum type;//数据的类型，GL_FLOAT等
GLboolean normalization = GL_FALSE;//是否把输入的数据归一化处理
std::string name;//定义几个，比如 "vertex"--顶点，"color"颜色，"normal" 法线
//long layoutid;//类似layout (location = 0) in vec2 position;中的0，定位数据的glsl的位置
};

static int getelementsize(GLenum type)
{
switch (type)
{
case GL_FLOAT:
return sizeof(GLfloat);
break;
case GL_INT:
return sizeof(GLint);
break;
case GL_UNSIGNED_INT:
return sizeof(GLuint);
break;
case GL_DOUBLE:
return sizeof(GLdouble);
break;
case GL_BYTE:
return sizeof(GLbyte);
break;
case GL_SHORT:
return sizeof(GLshort);
break;
case GL_BOOL:
return sizeof(GLboolean);
break;
case GL_UNSIGNED_BYTE:
return sizeof(GLubyte);
break;
case GL_UNSIGNED_SHORT:
return sizeof(GLushort);
break;
default:
break;
}

return 0;
}

#define  BUFFER_VERTEX   "vertex"
#define  BUFFER_COLOR    "color"
#define  BUFFER_NORMAL   "normal"


std::vector<Division> m_divisiontable;//表示buffer的意义
Division* getdataoffset(const char* name)
{
for (auto n : m_divisiontable)
{
if (n.name.compare(name) == 0)
return &n;
}
return nullptr;
}

*/