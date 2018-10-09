#include "query.h"


namespace GLUI
{


	bool Query::generate()
	{
		if (is_available())
			return true;

		glGenQueries(1, &m_queryID);
		LAZY_OPENGL_RETURN_FALSE("Query::generate: glGenQueries failed")
		return true;
	}

	bool Query::GetObject(QUEUE_NAME name, GLint * params​)
	{
		glGetQueryObjectiv(m_queryID, name, params​);
		LAZY_OPENGL_RETURN_FALSE("Query::GetObject: glGetQueryObjectiv failed")
		return true;
	}

	bool Query::GetObject(QUEUE_NAME name, GLuint* params​)
	{
		glGetQueryObjectuiv(m_queryID, name, params​);
		LAZY_OPENGL_RETURN_FALSE("Query::GetObject: glGetQueryObjectuiv failed")
		return true;
	}

	bool Query::GetObject(QUEUE_NAME name, GLint64 * params​)
	{
		glGetQueryObjecti64v(m_queryID, name, params​);
		LAZY_OPENGL_RETURN_FALSE("Query::GetObject: glGetQueryObjecti64v failed")
		return true;
	}

	bool Query::GetObject(QUEUE_NAME name, GLuint64  * params​)
	{
		glGetQueryObjectui64v(m_queryID, name, params​);
		LAZY_OPENGL_RETURN_FALSE("Query::GetObject: glGetQueryObjectui64v failed")
		return true;
	}

}