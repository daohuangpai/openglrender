#pragma once
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include "openglerror.h"

namespace GLUI
{


	class Query {
	public:
		virtual ~Query() { if(is_available()) glDeleteQueries(1,&m_queryID); }

		enum TARGET_TYPE
		{
			SAMPLES_PASSED = GL_SAMPLES_PASSED,//在绘制物体时使用beginQuery render EndQuery，然后测试QUERY_RESULT值，如果为0说明该物体被遮挡
			ANY_SAMPLES_PASSED = GL_ANY_SAMPLES_PASSED,
			ANY_SAMPLES_PASSED_CONSERVATIVE = GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
			PRIMITIVES_GENERATED = GL_PRIMITIVES_GENERATED,
			TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,//查询feeback最终写入的数据，因为可以使几何着色器，所以结果未知
			TIME_ELAPSED = GL_TIME_ELAPSED
		};

		enum QUEUE_NAME {
			QUERY_RESULT = GL_QUERY_RESULT,//表示通过了深度测试的片段数量，大于0则绘制，否则二次真正绘制丢弃该物体。，在feedback中就是等于输出的点了
			QUERY_RESULT_NO_WAIT = GL_QUERY_RESULT_NO_WAIT,
			QUERY_RESULT_AVAILABLE = GL_QUERY_RESULT_AVAILABLE
		};

		enum CONDITION_RENDER_TYPE
		{
			QUERY_WAIT = GL_QUERY_WAIT,//gpu等待直到查询QUERY_RESULT的结果出来
			QUERY_NO_WAIT = GL_QUERY_NO_WAIT,
			QUERY_BY_REGION_WAIT = GL_QUERY_BY_REGION_WAIT,
			QUERY_BY_REGION_NO_WAIT = GL_QUERY_BY_REGION_NO_WAIT
		};

		bool is_available() { return glIsQuery(m_queryID); }
		bool generate();

		bool beginQuery(TARGET_TYPE target) { glBeginQuery(target, m_queryID); LAZY_OPENGL_RETURN_FALSE("Query::beginQuery: glBeginQuery failed") return true; }
		bool EndQuery(TARGET_TYPE target) { glEndQuery(target); LAZY_OPENGL_RETURN_FALSE("Query::EndQuery: glEndQuery failed") return true; }
		

		//在之前SAMPLES_PASSED查找，并且使用渲染之间前后加入，将会自动根据上面SAMPLES_PASSED的结果GetObject结果决定接下来画不画
		bool BeginConditionalRender(CONDITION_RENDER_TYPE crt) { glBeginConditionalRender(m_queryID, crt); LAZY_OPENGL_RETURN_FALSE("Query::BeginConditionalRender: glBeginConditionalRender failed") return true;}
		bool EndConditionalRender() { glEndConditionalRender(); LAZY_OPENGL_RETURN_FALSE("Query::EndConditionalRender: glEndConditionalRender failed") return true; }

		bool GetObject(QUEUE_NAME name, GLint * params​);
		bool GetObject(QUEUE_NAME name, GLuint  * params​);
		bool GetObject(QUEUE_NAME name, GLint64 * params​);
		bool GetObject(QUEUE_NAME name, GLuint64  * params​);
	private:
		GLuint m_queryID;
	};



	/*
	https://www.khronos.org/opengl/wiki/GLAPI/glBeginConditionalRender
	https://blog.csdn.net/weixin_41254969/article/details/78706248?locationNum=5&fps=1 
	https://github.com/gecko0307/dagon/issues/8
	使用aabb来检查，然后画具体物体，这样简单多了，节省了渲染两次的其中一次时间
	然后检查的时候的渲染
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	关掉不必要的渲染
	Conditional rendering is an OpenGL 3.x feature. 
	The main idea behind it is rendering a simple bounding volume for object (like AABB) and testing 
	if any of it is actually visible on screen. If it is, then we can render the object itself, but if it isn't, 
	then we can safely discard the expensive rendering.

	// Initialization:
	GLuint query;
	glGenQueries(1​, &query​);

	// Rendering:
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glBeginQuery(GL_SAMPLES_PASSED, query);
	// render bounding volume of an object
	glEndQuery(GL_SAMPLES_PASSED);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glBeginConditionalRender(query, GL_QUERY_WAIT​);
	// render an object
	glEndConditionalRender();
	Conditional rendering can be an optional feature of Entity. 
	This will require an additional interface on top of Drawable which defines AABB property.
	*/
}