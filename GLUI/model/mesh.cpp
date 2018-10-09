#include "mesh.h"
#include <boost/log/trivial.hpp>
#include "..\common\commom.h"
namespace MODEL
{

    bool check(const mesh::arraybufferparam& param,std::map<mesh::MESH_BUFFER_TYPE, int>& map)
	{
		
		switch (param.type)
		{
		case mesh::VECTEX:
		case mesh::NORMAL:
		case mesh::TANGENTS:
		case mesh::BITANGENTS:
		{
			if (param.datatype != mesh::VEC3) {
				BOOST_LOG_TRIVIAL(error) << "check failed,just COLOR or TEXTURECOORDS support not VEC3";
				return false;
			}
			if (map.find(param.type) != map.end()) {
				BOOST_LOG_TRIVIAL(error) << "check failed,just COLOR or TEXTURECOORDS support same type,type:" << param.type;
				return false;
			}
		}
		break;
		case  mesh::COLOR:
		{
			if ((param.datatype < mesh::VEC3) || (param.datatype > mesh::VEC4))
			{
				BOOST_LOG_TRIVIAL(error) << "check failed,COLOR buffer just support 3 or 4 channle";
				return false;
			}
		}
		break;
		case mesh::TEXTURECOORDS:
		{
			if ((param.datatype < mesh::VEC1) || (param.datatype > mesh::VEC3))
			{
				BOOST_LOG_TRIVIAL(error) << "check failed,TEXTURECOORDS buffer just support 1 to 3 channle";
				return false;
			}
		}
		   break;
	    default:
		{
			BOOST_LOG_TRIVIAL(error) << "check failed, param.type error";
			return false;
		}
		  break;
	  }
		map[param.type] = 0;
		return true;
  }



	void mesh::clear()
	{
		m_vectex.reset();
		m_normal.reset();
		m_tangents.reset();
		m_bitangents.reset();
		m_color.clear();
		m_texturecoords.clear();
		m_AABB.zero();
	}

#define ARRAYTABLESBIND(table,T)\
    {\
	    table.push_back(std::move(std::unique_ptr<BufferArray>(new SimpleBufferArray<T>))); \
		SimpleBufferArray<T>* arraybuffer = (SimpleBufferArray<T>*)table.back().get(); \
        ARRAYBUFFERUPDATE((*arraybuffer),T)\
	}

	//#define ARRAYBUFFERUPDATE(arraybuffer,T) \
	//      {\
	//         arraybuffer.bindbuffer(buffer, begin, size);\
	//	     if (pm->width == 0) \
	//		 {\
	//		    CHECKFALSE(arraybuffer.Update((T*)pm->data, size, 0, false))\
	//	     }\
	//	     else \
	//		 {\
	//		   CHECKFALSE(arraybuffer.Updatewithoffset((char*)pm->data, pm->width, pm->offset, size, 0))\
	//	      }\
	//	     begin += arraybuffer.bytesize();\
	//         }
//和被注释的部分区别在于，这边为每一个不同组的数据分配一个buffer，而上面的是全部在同一个buffer中
#define ARRAYBUFFERUPDATE(arraybuffer,T) \
      {\
         std::shared_ptr<GLUI::buffer> vecbuffer;\
         SimpleBufferArray<T>::generatebuffer(size, GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER, vecbuffer);\
         arraybuffer.bindbuffer(vecbuffer, 0, size);\
\
	     if (pm->width == 0) \
		 {\
		    CHECKFALSE(arraybuffer.Update((T*)pm->data, size, 0, false))\
	     }\
	     else \
		 {\
		   CHECKFALSE(arraybuffer.Updatewithoffset((char*)pm->data, pm->width, pm->offset, size, 0))\
	      }\
	     begin += arraybuffer.bytesize();\
         }

	bool mesh::initarraybuffer(std::vector<arraybufferparam>& params, int size, bool clearbuffer)
	{
		if (clearbuffer)
			  clear();

		int bytesize = 0;
		std::map<mesh::MESH_BUFFER_TYPE, int> map;
		for (auto pm = params.begin(); pm != params.end(); pm++)
		{
			if (!check(*pm, map))
				return false;	 
			bytesize += pm->datatype * sizeof(float)*size;
		}

		if (map.find(VECTEX) == map.end())
		{
			BOOST_LOG_TRIVIAL(warning) << "mesh::init: no vectex?";
		}

		std::shared_ptr<buffer> buffer(new buffer);
		if (!buffer->generate(NULL, bytesize, buffer::STATIC_DRAW, buffer::ARRAY_BUFFER))
			return false;


		int begin = 0;
		for (auto pm = params.begin(); pm != params.end(); pm++)
		{
			switch (pm->type)
			{
			case VECTEX:
				ARRAYBUFFERUPDATE(m_vectex, glm::vec3)
				break;
			case NORMAL:
				ARRAYBUFFERUPDATE(m_normal, glm::vec3)
				break;
			case TANGENTS:
				ARRAYBUFFERUPDATE(m_tangents, glm::vec3)
				break;
			case BITANGENTS:
				ARRAYBUFFERUPDATE(m_bitangents, glm::vec3)
				break;
			case COLOR:
			    switch (pm->datatype)
				{
				   case VEC3:
					   ARRAYTABLESBIND(m_color, glm::vec3)
					    break;
				   case VEC4:
					   ARRAYTABLESBIND(m_color, glm::vec4)
						 break;
				   default:
					   BOOST_LOG_TRIVIAL(error) << "mesh::init,COLOR buffer just support 3 or 4 channle";
					   return false;
					   break;
				}
			   break;
			case TEXTURECOORDS:
				switch (pm->datatype)
				{
				case VEC1:
					ARRAYTABLESBIND(m_texturecoords, glm::vec1)
				    break;
				case VEC2:
					ARRAYTABLESBIND(m_texturecoords, glm::vec2)
				    break;
				case VEC3:
					ARRAYTABLESBIND(m_texturecoords, glm::vec3)
					break;
				default:
					BOOST_LOG_TRIVIAL(error) << "mesh::init,TEXTURECOORDS buffer just support 1 to 3 channle";
					return false;
					break;
			    }

			}//switch
		  }//for
		return generateAABB();
	}

#define ELEMENTTABLEBIND(T)\
    {   \
	   m_elements.push_back(std::move(std::unique_ptr<BufferArray>(new SimpleBufferArray<T>))); \
		SimpleBufferArray<T>* arraybuffer = (SimpleBufferArray<T>*)m_elements.back().get(); \
		arraybuffer->bindbuffer(buffer, begin, size); \
        CHECKFALSE(arraybuffer->Update((T*)pm->data, size, 0, false))\
		begin += arraybuffer->bytesize();\
	 }


	bool mesh::initelementbuffer(std::vector<elementybufferparam>& params)
	{
		unsigned int bytesize = 0;
		for (auto pm = params.begin(); pm != params.end(); pm++)
		{
			unsigned int perelementbytesize = Vao::elementtypetobytesize(pm->datatype);
			if (perelementbytesize == 0) {
				BOOST_LOG_TRIVIAL(error) << "mesh::initelementbuffer,error element type";
				return false;
			}
			bytesize +=perelementbytesize*pm->size;

		}

		std::shared_ptr<buffer> buffer(new buffer);
		if (!buffer->generate(NULL, bytesize, buffer::STATIC_DRAW, buffer::ELEMENT_ARRAY_BUFFER))
			return false;

		 
		int begin = 0;
		for (auto pm = params.begin(); pm != params.end(); pm++)
		{
			int size = pm->size;
			switch (pm->datatype)
			{
			case Vao::UNSIGNED_BYTE:
				 ELEMENTTABLEBIND(GLubyte)
				 break;
			case Vao::UNSIGNED_SHORT:
				ELEMENTTABLEBIND(GLushort)
				break;
			case Vao::UNSIGNED_INT:
				ELEMENTTABLEBIND(GLuint)
				break;
			default:
			   {
				BOOST_LOG_TRIVIAL(error) << "mesh::initelementbuffer,error element type";
				return false;
			   }
			   break;
			}
		}

		return UpdateElementAABB();
	}

#define ELEMENTAABB(type)\
	case sizeof(type) :\
	{\
		SimpleBufferArray<type>* GLubytebuffer = dynamic_cast<SimpleBufferArray<type>*>(buffer);\
		if (!GLubytebuffer->AccessData(\
			[&](type* data, int size)->bool\
		{\
			AABB& vecaabb = m_elementsAABB[pos];\
			for (int i = 0; i < size; i++)\
			{\
				type dataindex = data[i];\
				if (dataindex >= vectexsize) {\
					BOOST_LOG_TRIVIAL(error) << "mesh::UpdateElementAABB,element index out of vectex size:"<<(type)dataindex<<" "<<i;\
					return false;\
				}\
				glm::vec3& v = vectex[dataindex];\
				vecaabb.min = glm::vec3(min(vecaabb.min.x, v.x), min(vecaabb.min.y, v.y), min(vecaabb.min.z, v.z));\
				vecaabb.max = glm::vec3(max(vecaabb.max.x, v.x), max(vecaabb.max.y, v.y), max(vecaabb.max.z, v.z));\
			}\
			return true;\
		}\
		))\
		{\
			return false;\
		}\
	   }\
		break;

	bool mesh::UpdateElementAABB()
	{
		if (m_elements.size() == 0) 
			return true;
		if (!m_vectex.is_available())
		{
			BOOST_LOG_TRIVIAL(error) << "mesh::UpdateElementAABB,error no vectex";
			return false;
		}
		std::vector<glm::vec3> vectex;
		unsigned int vectexsize = m_vectex.size();
		vectex.resize(vectexsize);
		if (!m_vectex.CopyOut(&vectex[0], vectexsize, 0, true)) {
			BOOST_LOG_TRIVIAL(error) << "mesh::UpdateElementAABB,error cannot get vectex data";
			return false;
		}

		 m_elementsAABB.resize(m_elements.size());
		int pos = 0;
		int cpunt = 0;
		for (auto itr = m_elements.begin(); itr != m_elements.end(); itr++,pos++)
		{
			cpunt++;
			BufferArray* buffer = (*itr).get();
			switch (buffer->elementsize())
			{
				ELEMENTAABB(GLubyte)
				ELEMENTAABB(GLushort)
				ELEMENTAABB(GLuint)
				break;
			default:
				{
					BOOST_LOG_TRIVIAL(error) << "mesh::UpdateElementAABB,error element buffer type";
					return false;
				}
			}
		}
		return true;
	}

	bool mesh::generateAABB()
	{
		m_AABB.zero();
		if (!have_vectex())
			return true;

		return m_vectex.AccessData(
			[&](glm::vec3* data, int size)->bool
		{
			for (int i = 0; i < size; i++)
			{
				glm::vec3& v = data[i];
				m_AABB.min = glm::vec3(min(m_AABB.min.x, v.x),min(m_AABB.min.y, v.y), min(m_AABB.min.z, v.z));
				m_AABB.max = glm::vec3(max(m_AABB.max.x, v.x), max(m_AABB.max.y, v.y), max(m_AABB.max.z, v.z));
			}
			return true;
		}
		);
	}

	bool mesh::initbone(std::vector<glm::mat4>& mats, std::vector<bonetable>& bones, std::vector<int>& boneids)
	{
		{
			uint size = bones.size();
			std::shared_ptr<GLUI::buffer> buffer;
			CHECKFALSE(SimpleBufferArray<bonetable>::generatebuffer(size,
				GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, buffer))
			CHECKFALSE(m_bonetablebuffer.bindbuffer(buffer, 0, size))
			CHECKFALSE(m_bonetablebuffer.Update(&bones[0], size, 0, false))
		}
		{
			uint size = mats.size();
			std::shared_ptr<GLUI::buffer> buffer;
			CHECKFALSE(SimpleBufferArray<bonetable>::generatebuffer(size,
				GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, buffer))
				CHECKFALSE(m_bone_mats.bindbuffer(buffer, 0, size))
				CHECKFALSE(m_bone_mats.Update(&mats[0], size, 0, false))
		}
		{
			uint size = boneids.size();
			std::shared_ptr<GLUI::buffer> vecbuffer; 
			SimpleBufferArray<int>::generatebuffer(size, GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER, vecbuffer); 
			m_bonetableID.bindbuffer(vecbuffer, 0, size);
			CHECKFALSE(m_bonetableID.Update(&boneids[0], size, 0, false))
		}
		return true;
	}


//////////////////////////render///////////////////////////////////////////////////////////////////////////////////

//#define MESHRENDERARRAYBUFFERBIND(arraybuffername)\
//     {\
//	if (!mh->have_##arraybuffername##()) { BOOST_LOG_TRIVIAL(error) << " meshrender::bind no "<<#arraybuffername; return false; }\
//         m_vao.BindBuffer({ { mh->get_##arraybuffername##().buffer(), 0,pm->second,static_cast<GLint>(mh->get_##arraybuffername##().elementsize()/sizeof(float)),GL_FLOAT,GL_FALSE,0 } });\
//	}
//
//#define MESHRENDERARRAYBUFFERTABLEBIND(arraybuffername,pos)\
//{\
//	if (!mh->have_##arraybuffername##(pos)) { BOOST_LOG_TRIVIAL(error) << " meshrender::bind no " << #arraybuffername << " num:" << pos; return false; }\
//	m_vao.BindBuffer({ { mh->get_##arraybuffername##(pos)->buffer(), 0,pm->second,static_cast<GLint>(mh->get_##arraybuffername##(pos)->elementsize() / sizeof(float)),GL_FLOAT,GL_FALSE,0 } }); \
// }
// 
//
//	bool meshrendernormal::bind(mesh* mh, std::map<mesh::MESH_BUFFER_TYPE, GLuint>& locations, unsigned int colorpos, unsigned int texturepos)
//	{
//		for (auto pm = locations.begin(); pm != locations.end(); pm++)
//		{
//			switch (pm->first)
//			{
//			case mesh::VECTEX:
//				MESHRENDERARRAYBUFFERBIND(vectex)
//				break;
//			case mesh::NORMAL:
//				MESHRENDERARRAYBUFFERBIND(normal)
//				break;
//			case mesh::TANGENTS:
//				MESHRENDERARRAYBUFFERBIND(tangents)
//				break;
//			case mesh::BITANGENTS:
//				MESHRENDERARRAYBUFFERBIND(bitangents)
//				break;
//			case mesh::COLOR:
//				MESHRENDERARRAYBUFFERTABLEBIND(color, colorpos)
//				break;
//			case mesh::TEXTURECOORDS:
//				MESHRENDERARRAYBUFFERTABLEBIND(texturecoords, texturepos)
//				break;
//			default:
//			    {
//				  BOOST_LOG_TRIVIAL(error) << "mesh::initelementbuffer,error element type";
//				  return false;
//			    }
//				break;
//			}
//		}
//		return true;
//	}
//
//
//	bool meshrendernormal::drowarray(mesh* mh)
//	{
//		if (!m_vao.is_available())
//		{
//			BOOST_LOG_TRIVIAL(error) << "meshrender::drow: vao is not available";
//			return false;
//		}
//		if (!mh->have_vectex())
//		{
//			BOOST_LOG_TRIVIAL(error) << "meshrender::drow: no vectex ";
//			return false;
//		}
//		int drowsize = mh->get_vectex().size();
//		return m_vao.DrowArray(mh->get_drowtype(), 0, drowsize);
//
//	}
//
//	bool meshrendernormal::drowelement(mesh* mh, unsigned int elementpos)
//	{
//		if (!mh->have_elements(elementpos))
//		{
//			BOOST_LOG_TRIVIAL(error) << "meshrender::drow: no element " << elementpos << "cannot drow";
//			return false;
//		}
//
//		Vao::DROW_ELEMENT_DATA_TYPE elementdatatype;
//		switch (mh->get_elements(elementpos)->bytesize())
//		{
//		case sizeof(GLbyte) :
//			elementdatatype = Vao::UNSIGNED_BYTE;
//			break;
//		case sizeof(GLshort) :
//			elementdatatype = Vao::UNSIGNED_SHORT;
//			break;
//		case sizeof(GLint) :
//			elementdatatype = Vao::UNSIGNED_INT;
//			break;
//		}
//		return m_vao.Drawelements(mh->get_elements(elementpos)->buffer().get(), mh->get_drowtype(), mh->get_elements(elementpos)->size(), elementdatatype, 0);
//	}
//



}