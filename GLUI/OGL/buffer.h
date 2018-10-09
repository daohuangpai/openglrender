#pragma once
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include "mangertemplate.h"
#include "openglerror.h"
#include "util.h"
#include <boost/log/trivial.hpp>

namespace GLUI 
{
	

	class buffer
	{
	public:

		//STREAM  流表示初始化一次，而且使用次数比较少
		//STATIC 表示初始化一次，使用次数比较多
		//DYNAMIC 表示数据会被多次修改，而且使用的次数比较多
		//DRAW : 客户机指定了用于渲染的数据，并且使用于gl画图和特殊的图像命令
		//READ : 从OPENGL缓冲区读取数据值，并且在应用程序中用于各种与渲染不直接相关的计算过程
		//COPY : 从OPENGL缓冲区读取数据值, 并且使用于gl画图和特殊的图像命令
		enum USAGE_MODE {
			STATIC_DRAW = GL_STATIC_DRAW,
			STATIC_READ = GL_STATIC_READ,
			STATIC_COPY = GL_STATIC_COPY,
			STREAM_DRAW = GL_STREAM_DRAW,
			STREAM_READ = GL_STREAM_READ,
			STREAM_COPY = GL_STREAM_COPY,
			DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
			DYNAMIC_READ = GL_DYNAMIC_READ,
			DYNAMIC_COPY = GL_DYNAMIC_COPY,
		};

		enum TargetMode {
			ARRAY_BUFFER = GL_ARRAY_BUFFER,
			ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
			COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
			COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
			DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,
			DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
			ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
			PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
			PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
			QUERY_BUFFER = GL_QUERY_BUFFER,
			SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
			TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
			TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
			UNIFORM_BUFFER = GL_UNIFORM_BUFFER
		};
 
		virtual ~buffer() { glDeleteBuffers(1, &m_bufferid); }
		inline void bind() { 
			glBindBuffer(m_target,m_bufferid);
		}
		inline void Unbind() { glBindBuffer(m_target, 0); }

		//可以数据指针为NULL，这样就只在opengl断生成一个缓冲区
		virtual bool generate(char* data,int size, USAGE_MODE usagemode, TargetMode target);
		inline bool is_available() { return glIsBuffer(m_bufferid) == GL_TRUE; }

		//把原子缓存绑定到gsls的时候需要使用这个设定绑定的binding号。。binding = bindingnum，后面的offset则代表偏移位置的数据
		//原来是ARRAY_BUFFER也可以绑定为TRANSFORM_FEEDBACK_BUFFER
		bool BindToBinding(TargetMode mode,int bindingnum);
		bool BindBufferRange(TargetMode mode, int bindingnum,int beginindex,int size);
		bool copyfrombuffer(buffer* buffer,int srcoffset,int dstoffset,int size);

		GLint query_buffersize();
		GLint query_usage();
		GLint query_access();
		GLboolean query_mapped();

		
		bool UpdateDataHuge(char* data,int offset,int size);
		bool CopyOutDataHuge(char* data, int offset, int size);
		//if data is not huge  use glBufferSubData ？
		bool UpdateDataNotHuge(char* data, int offset, int size);
		bool CopyOutDataNotHuge(char* data, int offset, int size);
		//访问数据
		typedef void (*AccessFun)(char*, int, void*);
		bool AccessData(AccessFun fun,void* param);
		bool AccessData(const std::function<void(char*, int)> &AccessFuntion);

		char* mapping()
		{
			bind();
			char* ptr = (char*)glMapBuffer(m_target, GL_READ_WRITE);
			if (!opengl_error("glMapBuffer READ WRITE")) {
				return NULL;
			}
			return ptr;
		}
		char* mappingwriteRange(int offset, int size)
		{
			bind();
			char* ptr = (char*)glMapBufferRange(m_target, offset, size, GL_MAP_WRITE_BIT |GL_MAP_INVALIDATE_BUFFER_BIT);
			if (!opengl_error("glMapBuffer WRITE")) {
				return NULL;
			}
			return ptr;
		}
		char* mappingreadRange(int offset, int size)
		{
			bind();
			char* ptr = (char*)glMapBufferRange(m_target, offset, size,  GL_MAP_READ_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			if (!opengl_error("glMapBuffer READ")) {
				return NULL;
			}
			return ptr;
		}
		void Unmapping()
		{
			bind();
			glUnmapBuffer(m_target);
			Unbind();
		}

		USAGE_MODE get_usage() { return m_usage; }
		TargetMode get_target() { return m_target; }
		int getsize() { return m_element_size; }
		//如果tight那么分配空间将严格根据bytesize分配，不然按现有2倍增加分配
		//分配会拷贝旧的数据到头部
		bool ensuresize(int bytesize,bool tight = true);
	private:		
		int m_element_size;//数据的byte大小
		USAGE_MODE m_usage;
	   //buffer的类型，比如GL_ARRAY_BUFFER就是说这个数据是描述点，而GL_ELEMENT_ARRAY_BUFFER则是描述点的索引值
		//对一个vao先绑定GL_ARRAY_BUFFER，然后绑定GL_ELEMENT_ARRAY_BUFFER就是对索引点画图
		//glDrawArrays对应GL_ARRAY_BUFFER的画图，而glDrawElements对应GL_ELEMENT_ARRAY_BUFFER的画图，这里绑定的只是一个buffer的不同区域
		//也就是说一个buffer可以有多面和多种使用方式，就是在这里这个属性决定画图函数
		//为GL_ARRAY_BUFFER(表示顶点数据)、GL_ELEMENT_ARRAY_BUFFER(表示索引数据)、GL_PIXEL_PACK_BUFFER(表示传递给OpenGL像素数据)、
		//GL_PIXEL_UNPACK_BUFFER(表示从OpenGL获取的像素数据)、GL_COPY_READ_BUFFER和GL_COPY_WRITE_BUFFER(表示在缓冲区之间复制数据)、
		//GL_TEXTURE_BUFFER(表示作为纹理缓冲区存储的纹理数据)、GL_TRANSFORM_FEEDBACK_BUFFER(表示执行一个变换反馈着色器的结果)
		//或者GL_UNIFORM_BUFFER(表示统一变量值)
		TargetMode m_target;
	    GLuint m_bufferid = -1;
	};



	MangerTemplateClassPtr(std::string, buffer, buffermanger)
	//技巧 glMapBuffer()引起同步问题。如果GPU任然工作于该缓存对象，glMapBuffer()将一直等待直到GPU结束对应缓存对象上的工作。
	//	为了避免等待，你可以首先使用NULL调用glBufferData()，然后再调用glMapBuffer()。这样，前一个数据将被丢弃且glMapBuffer()立即返回一个新分配的指针，即使GPU任然工作于前一个数据。
	//	然而，由于你丢弃了前一个数据，这种方法只有在你想更新整个数据集的时候才有效。如果你仅仅希望更改部分数据或读取数据，你最好不要释放先前的数据。

	/*先绑定GL_ARRAY_BUFFER，然后绑定GL_ELEMENT_ARRAY_BUFFER渲染
	glGenVertexArrays(1, &m_nQuadVAO);
	glBindVertexArray(m_nQuadVAO);

	glGenBuffers(1, &m_nQuadPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_nQuadPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fQuadPos), fQuadPos, GL_STREAM_DRAW);

	glEnableVertexAttribArray(VAT_POSITION);
	glVertexAttribPointer(VAT_POSITION, 2, GL_INT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &m_nQuadTexcoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_nQuadTexcoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fQuadTexcoord), fQuadTexcoord, GL_STREAM_DRAW);

	glEnableVertexAttribArray(VAT_TEXCOORD);
	glVertexAttribPointer(VAT_TEXCOORD, 2, GL_INT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &m_nQuadIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nQuadIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(nQuadIndex), nQuadIndex, GL_STREAM_DRAW);

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	渲染时候：
	glBindVertexArray(m_nQuadVAO);
	// 使用了VAO更多使用glDrawArrays，因为提供了一个渲染数组，
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(NULL);
	*/



}