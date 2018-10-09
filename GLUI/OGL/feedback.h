#pragma once
#include "buffer.h"
#include "shader.h"
#include "Vao.h"
namespace GLUI
{

	class transformfeedback {
	public:
		enum BUFFER_OUT_TYPE
		{
			////����������д��һ�����������󡣡������ܵĻ����ֻ��һ������������� out vec3 a  out vec3 b
			//��ô�����Խ�ʹ�������ʽΪa1[0]a1[1]a1[2]b1[0]b1[1]b1[2]a2[0]a2[1]a2[2]...������������ݣ�������ľͽ���ͬ�����ݷָ�����������ݣ�a����a��b����b
			INTERLEAVED_ATTRIBS = GL_INTERLEAVED_ATTRIBS,
			SEPARATE_ATTRIBS = GL_SEPARATE_ATTRIBS//������д�������������󣬻򽫲�ͬ��ƫ����д�뻺������
		};
		virtual ~transformfeedback();
		bool generate();
		//��������൱�ڳ�ʼ��;
		//program ��vectex����fragment��program֮ǰ���øú�����ʼ����programΪshader���
		//outnameΪglsl��д��out �ı�������Ҳ���������
		//���� "gl_SkipComponents1"-��"gl_SkipComponents4"��ʾҪ�������ٸ��ռ䣬��λΪ1
		//"gl_NextBuffer"Ϊ������һ���󶨵����buffer glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);�еĲ���0,1��2.����Ĭ�ϴ�0��ʼ
		bool init(GLuint program, BUFFER_OUT_TYPE outtype, char** outname, int outnamesize);


		inline bool transformfeedback::bind()
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformfeedbackid);//Ŀǰogl�汾4.6ֻ֧��GL_TRANSFORM_FEEDBACK
			LAZY_OPENGL_RETURN_FALSE("transformfeedback::bind: glBindTransformFeedback failed")
				return true;
		}
		inline bool is_available() { return glIsTransformFeedback(m_transformfeedbackid) == GL_TRUE; }
		//ע�⣬��ֻͣ��ִ��drow��ʱ��д�뻺�����ˣ��������û��ʹ��glEnable(GL_RASTERIZER_DISCARD);�رչ�դ���ɻ�ͼ�����Ҽ���Ҳ������
		//ֻ�ǲ�д���ú�ͣ��һ��������Ѿ���PauseTransformFeedback�ڵ���PauseTransformFeedback��ʧ��
		bool PauseTransformFeedback();
		bool ResumeTransformFeedback();

		//�����bindֻ������������buffer�������buffer����������û��"gl_NextBuffer"����
		bool BindOutPutBuffer(std::shared_ptr<buffer>& buffer, int num) { if (!bind()) return false;  return buffer->BindToBinding(buffer::TRANSFORM_FEEDBACK_BUFFER, num); }
		bool Drow(Vao* vao, Vao::DROW_TYPE drowtype, int databegin, int data_size);
	private:
		GLuint m_transformfeedbackid = -1;
	};


//class transformfeedback {
//	public:
//		enum BUFFER_OUT_TYPE{
//		////����������д��һ�����������󡣡������ܵĻ����ֻ��һ������������� out vec3 a  out vec3 b
//		//��ô�����Խ�ʹ�������ʽΪa1[0]a1[1]a1[2]b1[0]b1[1]b1[2]a2[0]a2[1]a2[2]...������������ݣ�������ľͽ���ͬ�����ݷָ�����������ݣ�a����a��b����b
//		INTERLEAVED_ATTRIBS = GL_INTERLEAVED_ATTRIBS,
//		SEPARATE_ATTRIBS = GL_SEPARATE_ATTRIBS//������д�������������󣬻򽫲�ͬ��ƫ����д�뻺������
//		};
//		//���ʹ�ü�����ɫ�����򷵻ص����ݾ��Ǽ�����ɫ��������
//		bool init(char* vectex,char* fragment,char** outname,int outnamesize, BUFFER_OUT_TYPE type, const std::function<bool(void)> &initVao);
//		bool run(Vao::DROW_TYPE drowtype, int databegin, int data_size);
//		 
//	private:
//		rendershader m_feekbackshader;
//		Vao m_vao;
//};

/*
TransformFeedback��OpenGL�����У����㴦��(����м�����ɫ������ô���ڼ�����ɫ����)֮��
ͼԪװ��֮ǰ��һ�����衣���������²��񼴽�װ��ΪͼԪ(�㣬�߶Σ�������)�Ķ��㣬Ȼ�󽫲��ֻ�ȫ�����Դ��ݵ��������������
ÿ��һ�����㴫�ݵ�ͼԪװ��׶�ʱ����������Ҫ��������Լ�¼��һ��������������ϣ��û�������Իض���Щ�����������ݡ�
*/

}