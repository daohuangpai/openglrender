#include "vao.h"
namespace GLUI
{
	MangerTemplateClassCPP(Vaomanger)

	bool Vao::BindBuffer(std::initializer_list<VaoBindBufferStruct> VaoBindBufferStructs)
	{
		generate();
		for (auto Bufferinfo = VaoBindBufferStructs.begin(); Bufferinfo != VaoBindBufferStructs.end(); Bufferinfo++)
		{
			if (m_bufferlist.find(Bufferinfo->layoutindex) != m_bufferlist.end()) {
				BOOST_LOG_TRIVIAL(warning) << "Vao::UpdateBufferTable: bind to same layoutindex:" << Bufferinfo->layoutindex << ",only the last bind work";
			}
			m_bufferlist[Bufferinfo->layoutindex] = std::move(*Bufferinfo);
		}
		return true;
	}


	//因为绑定点的buffer是唯一的，每一次调用
	//glVertexAttribPointer这里只使用这个，实际上有矩阵数据等一系列和整型的glVertexAttribIPointer
	bool Vao::autobindbufferbeforedrow()
	{
		for (auto table = m_bufferlist.begin(); table != m_bufferlist.end(); table++)
		{
			VaoBindBufferStruct* Bufferinfo = &table->second;
			if (!Bufferinfo->buf.get() || !Bufferinfo->buf->is_available()) {
				BOOST_LOG_TRIVIAL(error) << "Vao::autobindbufferbeforedrow: cannot BindBuffer with unable buffer!";
				return false;
			}
			Bufferinfo->buf->bind();
			glEnableVertexAttribArray(Bufferinfo->layoutindex);
			LAZY_OPENGL_RETURN_FALSE("Vao::autobindbufferbeforedrow: glEnableVertexAttribArray failed")
			glVertexAttribPointer(Bufferinfo->layoutindex, Bufferinfo->size, Bufferinfo->DataType, Bufferinfo->normalized, Bufferinfo->width, (void*)Bufferinfo->offset);
			LAZY_OPENGL_RETURN_FALSE("Vao::autobindbufferbeforedrow: glVertexAttribPointer failed")	
		}
		return true;
	}

	bool Vao::autobindbufferbeafterdrow()
	{
		for (auto table = m_bufferlist.begin(); table != m_bufferlist.end(); table++)
		{
			VaoBindBufferStruct* Bufferinfo = &table->second;
			glDisableVertexAttribArray(Bufferinfo->layoutindex);
			LAZY_OPENGL_RETURN_FALSE("Vao::autobindbufferbeafterdrow: glDisableVertexAttribArray failed")
		}
		return true;
	}

	bool Vao::DrowArray(DROW_TYPE drowtype, int first, int num)
	{ 
		bind(); 
		autobindbufferbeforedrow();
		LAZY_OPENGL_RETURN_FALSE("Vao::DrowArray: bind failed")
		glDrawArrays(drowtype, first, num); 
		LAZY_OPENGL_RETURN_FALSE("Vao::DrowArray: glDrawArrays failed")
		return autobindbufferbeafterdrow();
	}

	bool Vao::Drawelements(buffer* elementbuffer, DROW_TYPE drowtype, GLsizei count​, DROW_ELEMENT_DATA_TYPE type​, const GLvoid * indices​)
	{
		if ((elementbuffer == nullptr) || !elementbuffer->is_available()) {
			BOOST_LOG_TRIVIAL(error) << "Vao::Drawelements: cannot BindBuffer with unable buffer!";
			return false;
		}
		bind();
		autobindbufferbeforedrow();
		LAZY_OPENGL_RETURN_FALSE("Vao::DrowArray: bind failed")
		elementbuffer->bind();
		glDrawElements(drowtype, count​, type​, indices​);
		LAZY_OPENGL_RETURN_FALSE("Vao::DrowArray: glDrawArrays failed")
		return autobindbufferbeafterdrow();
	}



	unsigned int Vao::elementtypetobytesize(Vao::DROW_ELEMENT_DATA_TYPE type)
	{
		int result = 0;
		switch (type)
		{
		case Vao::UNSIGNED_BYTE:
			result = sizeof(GLubyte);
			break;
		case Vao::UNSIGNED_SHORT:
			result = sizeof(GLushort);
			break;
		case Vao::UNSIGNED_INT:
			result = sizeof(GLuint);
			break;
		default:
			break;
		}
		return result;
	}


	bool Vaodynamic::bindvectexbuffer(std::shared_ptr<buffer> buf,//
		  unsigned int width,//取下一行的数据量，如果width = 0表明数据是紧密对齐的
		  GLuint  layoutindex,//glsl layout num
		  GLint  size,//根据DataType类型的个数，不是以byte为单位
		  GLenum DataType,//指示数据的的类型
		  GLboolean normalized,//是否把数据归一化
		  unsigned int  offset//对同一行数据偏移多少开始取，相对于sizeof(DataType)* widthsize)
	)
	{
		buf->bind();
		glVertexAttribPointer(layoutindex, size, DataType, normalized, width, (void*)offset);
		LAZY_OPENGL_RETURN_FALSE("Vao::bindbuffer: glVertexAttribPointer failed")
		return true;
	}
	

	bool Vaodynamic::DrowArray(Vao::DROW_TYPE drowtype, int first, int num)
	{
		glDrawArrays(drowtype, first, num);
		return opengl_error("Vaodynamic::DrowArray");
	}

	bool Vaodynamic::Drawelements(buffer* elementbuffer, Vao::DROW_TYPE drowtype, GLsizei count​, Vao::DROW_ELEMENT_DATA_TYPE type​, const GLvoid * indices​)
	{
		elementbuffer->bind();
		glDrawElements(drowtype, count​, type​, indices​);
		return opengl_error("Vaodynamic::Drawelements");
	}
}