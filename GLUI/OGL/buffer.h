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

		//STREAM  ����ʾ��ʼ��һ�Σ�����ʹ�ô����Ƚ���
		//STATIC ��ʾ��ʼ��һ�Σ�ʹ�ô����Ƚ϶�
		//DYNAMIC ��ʾ���ݻᱻ����޸ģ�����ʹ�õĴ����Ƚ϶�
		//DRAW : �ͻ���ָ����������Ⱦ�����ݣ�����ʹ����gl��ͼ�������ͼ������
		//READ : ��OPENGL��������ȡ����ֵ��������Ӧ�ó��������ڸ�������Ⱦ��ֱ����صļ������
		//COPY : ��OPENGL��������ȡ����ֵ, ����ʹ����gl��ͼ�������ͼ������
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

		//��������ָ��ΪNULL��������ֻ��opengl������һ��������
		virtual bool generate(char* data,int size, USAGE_MODE usagemode, TargetMode target);
		inline bool is_available() { return glIsBuffer(m_bufferid) == GL_TRUE; }

		//��ԭ�ӻ���󶨵�gsls��ʱ����Ҫʹ������趨�󶨵�binding�š���binding = bindingnum�������offset�����ƫ��λ�õ�����
		//ԭ����ARRAY_BUFFERҲ���԰�ΪTRANSFORM_FEEDBACK_BUFFER
		bool BindToBinding(TargetMode mode,int bindingnum);
		bool BindBufferRange(TargetMode mode, int bindingnum,int beginindex,int size);
		bool copyfrombuffer(buffer* buffer,int srcoffset,int dstoffset,int size);

		GLint query_buffersize();
		GLint query_usage();
		GLint query_access();
		GLboolean query_mapped();

		
		bool UpdateDataHuge(char* data,int offset,int size);
		bool CopyOutDataHuge(char* data, int offset, int size);
		//if data is not huge  use glBufferSubData ��
		bool UpdateDataNotHuge(char* data, int offset, int size);
		bool CopyOutDataNotHuge(char* data, int offset, int size);
		//��������
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
		//���tight��ô����ռ佫�ϸ����bytesize���䣬��Ȼ������2�����ӷ���
		//����´���ɵ����ݵ�ͷ��
		bool ensuresize(int bytesize,bool tight = true);
	private:		
		int m_element_size;//���ݵ�byte��С
		USAGE_MODE m_usage;
	   //buffer�����ͣ�����GL_ARRAY_BUFFER����˵��������������㣬��GL_ELEMENT_ARRAY_BUFFER���������������ֵ
		//��һ��vao�Ȱ�GL_ARRAY_BUFFER��Ȼ���GL_ELEMENT_ARRAY_BUFFER���Ƕ������㻭ͼ
		//glDrawArrays��ӦGL_ARRAY_BUFFER�Ļ�ͼ����glDrawElements��ӦGL_ELEMENT_ARRAY_BUFFER�Ļ�ͼ������󶨵�ֻ��һ��buffer�Ĳ�ͬ����
		//Ҳ����˵һ��buffer�����ж���Ͷ���ʹ�÷�ʽ������������������Ծ�����ͼ����
		//ΪGL_ARRAY_BUFFER(��ʾ��������)��GL_ELEMENT_ARRAY_BUFFER(��ʾ��������)��GL_PIXEL_PACK_BUFFER(��ʾ���ݸ�OpenGL��������)��
		//GL_PIXEL_UNPACK_BUFFER(��ʾ��OpenGL��ȡ����������)��GL_COPY_READ_BUFFER��GL_COPY_WRITE_BUFFER(��ʾ�ڻ�����֮�临������)��
		//GL_TEXTURE_BUFFER(��ʾ��Ϊ���������洢����������)��GL_TRANSFORM_FEEDBACK_BUFFER(��ʾִ��һ���任������ɫ���Ľ��)
		//����GL_UNIFORM_BUFFER(��ʾͳһ����ֵ)
		TargetMode m_target;
	    GLuint m_bufferid = -1;
	};



	MangerTemplateClassPtr(std::string, buffer, buffermanger)
	//���� glMapBuffer()����ͬ�����⡣���GPU��Ȼ�����ڸû������glMapBuffer()��һֱ�ȴ�ֱ��GPU������Ӧ��������ϵĹ�����
	//	Ϊ�˱���ȴ������������ʹ��NULL����glBufferData()��Ȼ���ٵ���glMapBuffer()��������ǰһ�����ݽ���������glMapBuffer()��������һ���·����ָ�룬��ʹGPU��Ȼ������ǰһ�����ݡ�
	//	Ȼ���������㶪����ǰһ�����ݣ����ַ���ֻ������������������ݼ���ʱ�����Ч����������ϣ�����Ĳ������ݻ��ȡ���ݣ�����ò�Ҫ�ͷ���ǰ�����ݡ�

	/*�Ȱ�GL_ARRAY_BUFFER��Ȼ���GL_ELEMENT_ARRAY_BUFFER��Ⱦ
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
	��Ⱦʱ��
	glBindVertexArray(m_nQuadVAO);
	// ʹ����VAO����ʹ��glDrawArrays����Ϊ�ṩ��һ����Ⱦ���飬
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(NULL);
	*/



}