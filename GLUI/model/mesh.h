#pragma once
#include "../OGL/util.h"
#include "../OGL/commonbuffer.h"
#include "../OGL/vao.h"
#include "../OGL/lazytool.h"
#include "animation.h"
#include <map>
using namespace GLUI;

namespace MODEL
{
	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
		glm::vec3 size() { return max - min; }

		static AABB combine(AABB& a, AABB& b)
		{
			AABB ret;
			ret.min = glm::vec3(min(a.min.x, b.min.x), min(a.min.y, b.min.y), min(a.min.z, b.min.z));
			ret.max = glm::vec3(max(a.max.x, b.max.x), max(a.max.y, b.max.y), max(a.max.z, b.max.z));
			return ret;
		}

		void zero() { min = max = glm::vec3(0.0f, 0.0f, 0.0f); }
	};


	//�������⣬����֧��float����
	class mesh  {
	public:
		enum MESH_BUFFER_TYPE
		{
			VECTEX = 0,
			NORMAL,
			TANGENTS,
			BITANGENTS,
			COLOR,
			TEXTURECOORDS
		};
		enum DATA_TYPE
		{
			 VEC1 = 1 ,
			 VEC2 = 2,
			 VEC3 = 3,
			 VEC4 = 4
		};
		//VECTEX-BITANGENTS datatype�������VEC3
		//COLOR  datatype����VEC3����VEC4
		//TEXTURECOORDS  datatype����VEC1-VEC3

		struct arraybufferparam {
			MESH_BUFFER_TYPE type;
			float* data;
			DATA_TYPE datatype;//ÿһ�����ݵ����ݴ�С����vec,��ʾһ�����ݵĶ���
			int offset;//���width��Ϊ0�����ֵ����ÿһ��i*widthƫ��λ�üӶ��ٿ�ʼ
			int width;//ʵ��������һ�е�ʵ��ƫ���������ֽ�Ϊ��λ,Ϊ0��Ϊ��������
		};
		AABB get_AABB() { return m_AABB; }
		bool generateAABB();
		//size --ͳһ����������ͷ��ߵ���������һ�µ�
		bool initarraybuffer(std::vector<arraybufferparam>& params,int size,bool clearbuffer = true);

		struct elementybufferparam
		{
			Vao::DROW_ELEMENT_DATA_TYPE datatype;
			void* data;
			int size;
		};
		bool initelementbuffer(std::vector<elementybufferparam>& params);
		bool initbone(std::vector<glm::mat4>& mats, std::vector<bonetable>& bones, std::vector<int>& boneids);
		bool UpdateElementAABB();
		SET_GET_FUN(Vao::DROW_TYPE, drowtype)
		void clear();
		typedef  std::vector<std::shared_ptr<BufferArray>> buffer_vector;

#define HAVEPOINTBUFFER(NAME,TYPE)\
		bool have_##NAME##(){return (m_##NAME##.is_available()&&m_##NAME##.size()>0);}\
		SimpleBufferArray<TYPE>& get_##NAME##(){ return m_##NAME##; }

		HAVEPOINTBUFFER(vectex, glm::vec3)
		HAVEPOINTBUFFER(normal, glm::vec3)
		HAVEPOINTBUFFER(tangents, glm::vec3)
		HAVEPOINTBUFFER(bitangents, glm::vec3)

		HAVEPOINTBUFFER(bonetablebuffer,bonetable)
		HAVEPOINTBUFFER(bone_mats, glm::mat4)
		HAVEPOINTBUFFER(bonetableID,int)

#define MAP_HAVEPOINTBUFFER(NAME)\
		bool have_##NAME##(int pos){\
			if(m_##NAME##.size()>pos)\
            { \
			  return (m_##NAME##[pos].get()&&m_##NAME##[pos]->is_available());\
			}\
			return false;\
		}\
		BufferArray*  get_##NAME##(int pos)\
		{\
			if (m_##NAME##.size()>pos)\
            { \
			  return m_##NAME##[pos].get();\
			}\
            return nullptr;\
		}\
        buffer_vector& get_##NAME##_table(){return m_##NAME##;}

		MAP_HAVEPOINTBUFFER(color)
		MAP_HAVEPOINTBUFFER(texturecoords)
		MAP_HAVEPOINTBUFFER(elements)
			
		struct meshparam
		{
			SimpleBufferArray<glm::vec3>* vectex = nullptr;
			SimpleBufferArray<glm::vec3>* normal = nullptr;
			SimpleBufferArray<glm::vec3>* tangents = nullptr;
			SimpleBufferArray<glm::vec3>* bitangents = nullptr;
			BufferArray* color = nullptr;
			BufferArray* texturecoords = nullptr;
			BufferArray* elements = nullptr;
		};

		struct config
		{
			unsigned int colorindex;
			unsigned int texturecoordsindex;
		};
	private:
		Vao::DROW_TYPE m_drowtype = Vao::TRIANGLES;
		
		SimpleBufferArray<glm::vec3> m_vectex;
		SimpleBufferArray<glm::vec3> m_normal;
		SimpleBufferArray<glm::vec3> m_tangents;
		SimpleBufferArray<glm::vec3> m_bitangents;

		SimpleBufferArray<bonetable> m_bonetablebuffer;
		SimpleBufferArray<glm::mat4> m_bone_mats;
		SimpleBufferArray<int> m_bonetableID;

		buffer_vector m_color;
		buffer_vector m_texturecoords;
		AABB m_AABB;

		buffer_vector m_elements;
		std::vector<AABB> m_elementsAABB;
	};


	//class meshrender {
	//public:
	//	enum TYPE
	//	{
	//		NORMAL = 0
	//	};
	//	virtual TYPE type() = 0;
	//};

	//class meshrendernormal : public meshrender {
	//public:
	//	virtual TYPE type() { return NORMAL; }
	//	bool drowelement(mesh* mh, unsigned int elementpos);
	//	bool drowarray(mesh* mh);
	//	bool bind(mesh* mh, std::map<mesh::MESH_BUFFER_TYPE, GLuint>& locations, unsigned int colorpos,unsigned int texturepos);
	//private:
	//	Vao m_vao;
	//};
}
