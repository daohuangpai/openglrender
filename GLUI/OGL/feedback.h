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
			////将所有属性写入一个缓冲区对象。。即接受的缓冲的只有一个，比如输出是 out vec3 a  out vec3 b
			//那么该属性将使得输出格式为a1[0]a1[1]a1[2]b1[0]b1[1]b1[2]a2[0]a2[1]a2[2]...这样交错的数据，而下面的就将不同的数据分割开成连续的数据，a就是a，b就是b
			INTERLEAVED_ATTRIBS = GL_INTERLEAVED_ATTRIBS,
			SEPARATE_ATTRIBS = GL_SEPARATE_ATTRIBS//将属性写入多个缓冲区对象，或将不同的偏移量写入缓冲区。
		};
		virtual ~transformfeedback();
		bool generate();
		//这个函数相当于初始化;
		//program 在vectex或者fragment绑定program之前调用该函数初始化，program为shader句柄
		//outname为glsl中写的out 的变量名，也就是输出名
		//名称 "gl_SkipComponents1"-》"gl_SkipComponents4"表示要跳过多少个空间，单位为1
		//"gl_NextBuffer"为调到下一个绑定的输出buffer glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);中的参数0,1、2.。。默认从0开始
		bool init(GLuint program, BUFFER_OUT_TYPE outtype, char** outname, int outnamesize);


		inline bool transformfeedback::bind()
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformfeedbackid);//目前ogl版本4.6只支持GL_TRANSFORM_FEEDBACK
			LAZY_OPENGL_RETURN_FALSE("transformfeedback::bind: glBindTransformFeedback failed")
				return true;
		}
		inline bool is_available() { return glIsTransformFeedback(m_transformfeedbackid) == GL_TRUE; }
		//注意，暂停只是执行drow的时候不写入缓冲区了，但是如果没有使用glEnable(GL_RASTERIZER_DISCARD);关闭光栅依旧画图，而且计算也依旧在
		//只是不写入搞得和停了一样，如果已经是PauseTransformFeedback在调用PauseTransformFeedback会失败
		bool PauseTransformFeedback();
		bool ResumeTransformFeedback();

		//这里的bind只是相对于输出的buffer，输出的buffer个数根据有没有"gl_NextBuffer"决定
		bool BindOutPutBuffer(std::shared_ptr<buffer>& buffer, int num) { if (!bind()) return false;  return buffer->BindToBinding(buffer::TRANSFORM_FEEDBACK_BUFFER, num); }
		bool Drow(Vao* vao, Vao::DROW_TYPE drowtype, int databegin, int data_size);
	private:
		GLuint m_transformfeedbackid = -1;
	};


//class transformfeedback {
//	public:
//		enum BUFFER_OUT_TYPE{
//		////将所有属性写入一个缓冲区对象。。即接受的缓冲的只有一个，比如输出是 out vec3 a  out vec3 b
//		//那么该属性将使得输出格式为a1[0]a1[1]a1[2]b1[0]b1[1]b1[2]a2[0]a2[1]a2[2]...这样交错的数据，而下面的就将不同的数据分割开成连续的数据，a就是a，b就是b
//		INTERLEAVED_ATTRIBS = GL_INTERLEAVED_ATTRIBS,
//		SEPARATE_ATTRIBS = GL_SEPARATE_ATTRIBS//将属性写入多个缓冲区对象，或将不同的偏移量写入缓冲区。
//		};
//		//如果使用几何着色器，则返回的数据就是几何着色器产生的
//		bool init(char* vectex,char* fragment,char** outname,int outnamesize, BUFFER_OUT_TYPE type, const std::function<bool(void)> &initVao);
//		bool run(Vao::DROW_TYPE drowtype, int databegin, int data_size);
//		 
//	private:
//		rendershader m_feekbackshader;
//		Vao m_vao;
//};

/*
TransformFeedback是OpenGL管线中，顶点处理(如果有几何着色器，那么就在几何着色器后)之后，
图元装配之前的一个步骤。它可以重新捕获即将装配为图元(点，线段，三角形)的顶点，然后将部分或全部属性传递到缓存对象上来。
每当一个顶点传递到图元装配阶段时，将所有需要捕获的属性记录到一个或多个缓存对象上，用户程序可以回读这些缓存对象的内容。
*/

}