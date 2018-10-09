#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../OGL/treestruct.h"
#include "../OGL/util.h"
#include "../OGL/commonbuffer.h"
#include "../OGL/vao.h"
#include <map>

namespace GLUI 
{
	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
		glm::vec3 size() { return max - min; }

		static AABB combine(AABB& a, AABB& b);
		static void zero(AABB& AABB);
	};
	

	class assimpmesh {
	public:

		bool init(aiMesh* mesh);
		inline bool is_available() { return m_vectex.size()>0; }
		void clear();
		void drow();
	private:
		Vao::DROW_TYPE m_drowtype;//��Ӧaimesh��mPrimitiveTypes�������Ч����aiPrimitiveType_POINT��aiPrimitiveType_LINE��aiPrimitiveType_TRIANGLE��aiPrimitiveType_POLYGON
		std::shared_ptr<buffer> m_arraybuffer;
		std::shared_ptr<buffer> m_element;
		Vao::DROW_ELEMENT_DATA_TYPE elementtype = Vao::UNSIGNED_INT;//assimpʹ�õ�������������͵�

		SimpleBufferArray<glm::vec3> m_vectex;
		SimpleBufferArray<glm::vec3> m_normal;
		SimpleBufferArray<glm::vec3> m_Tangents;
		SimpleBufferArray<glm::vec3> m_Bitangents;
		std::vector<SimpleBufferArray<glm::vec4>> m_color;
		std::vector<std::shared_ptr<BufferArray>> m_texturecoords;
		AABB get_AABB() { return m_AABB; }
        //
		//std::vector<SampleBufferArray<unsigned int>> m_elementIndexs;
		SimpleBufferArray<unsigned int> m_elementIndexs;
		Vao m_vao;
		unsigned int m_MaterialIndex;//��Ӧ��������(std::vector<assimpMaterial>)
		AABB m_AABB;
		friend class assimploader;
	};


	class assimpMaterial {
	public:
		bool init(aiMaterial* material, std::string rootpath);
	private:
		std::map<aiTextureType, std::map<unsigned int, std::shared_ptr<Texture2D>>> m_Materials;
		friend class assimploader;
	};
	


	// image* assimpTexture(aiTexture* texture);
 

    class assimpnode :public simpletree<assimpnode> {
     public:
		 void init(aiNode* node);
     private:
		 glm::mat4 m_transformation;
		 std::string m_name;
		 std::vector<unsigned int> m_Meshes;
		 friend class assimploader;
   };

	class assimploader {
	public: 
		  bool Load(std::string path,int loadtype = aiProcess_Triangulate | aiProcess_CalcTangentSpace);
		 // aiScene* sence() { return m_aiScene.get(); }
		  void drow(std::shared_ptr<shader>& shader, glm::mat4& transformationmat);
		  void drownode(assimpnode* node, std::shared_ptr<shader>& shader, glm::mat4& transformationmat);
		  void compute_AABB(assimpnode* node);

		  AABB get_AABB() { return m_aabb; }
	private:
		std::vector<assimpmesh> m_meshs;
		std::vector<assimpMaterial> m_Material;
		//std::unique_ptr<aiScene> m_aiScene;
		std::unique_ptr<assimpnode> m_noderoot;
		AABB m_aabb;
	};


}

//class assimpMaterial {
//public:
//	bool init(aiMaterial* material);
//	void clear();
//	enum MATERIAL_TYPE
//	{
//		NONE = aiTextureType_NONE,
//		DIFFUSE = aiTextureType_DIFFUSE,
//		SPECULAR = aiTextureType_SPECULAR,
//		AMBIENT = aiTextureType_AMBIENT,
//		EMISSIVE = aiTextureType_EMISSIVE,
//		HEIGHT = aiTextureType_HEIGHT,
//		NORMALS = aiTextureType_NORMALS,
//		SHININESS = aiTextureType_SHININESS,
//		OPACITY = aiTextureType_OPACITY,
//		DISPLACEMENT = aiTextureType_DISPLACEMENT,
//		LIGHTMAP = aiTextureType_LIGHTMAP,
//		REFLECTION = aiTextureType_REFLECTION,
//		UNKNOWN = aiTextureType_UNKNOWN,
//		NOUSE = _aiTextureType_Force32Bit
//	};
//	//struct aiMaterialProperty
//	//{
//	//	C_STRUCT aiString mKey;//���ƣ�����AI_MATKEY_NAME��_AI_MATKEY_TEXTURE_BASE���������
//	//	unsigned int mSemantic;//MATERIAL_TYPE��������,aiTextureType_NONE��ʾ�������޹ص����ã���ô��������������������й�
//	//	unsigned int mIndex;//������ͬ������MATERIAL_TYPE�������£��ڼ�������ʾ��ͬ�����õĵڼ���
//	//	unsigned int mDataLength;//���ݵ��ֽڴ�С
//	//	C_ENUM aiPropertyTypeInfo mType;//���ݵ�����
//	//	char* mData;//���ݵ�ָ��
//	//};
//	struct Material_Element
//	{
//		unsigned int mIndex;//�ڼ�����ͬ���͵�����
//		Value value;
//		aiPropertyTypeInfo type;
//		std::shared_ptr<Texture> tex;
//	};
//
//	//��������������
//	//������ַ�����
//	std::map<MATERIAL_TYPE, std::map<std::string, std::map<unsigned int, Material_Element>>> m_MaterialProperty;
//};