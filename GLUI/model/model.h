#pragma once
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "../OGL/light.h"
#include "../common/configure.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace MODEL
{

	class model
	{
	public:

#define GET_SET_FORM_VECTOR_UNIQUEPTR(type,table)\
		type* get_##table##(unsigned int pos)\
	    {\
			if (m_##table##.size() <= pos)\
				return nullptr;\
			return m_##table##[pos].get();\
		}\
		bool set_##table##(type* v,unsigned int pos)\
        {\
			if (m_##table##.size() <= pos)\
				return false;\
			 m_##table##[pos].reset(v);\
             return true;}



		GET_SET_FORM_VECTOR_UNIQUEPTR(mesh, meshs)
		GET_SET_FORM_VECTOR_UNIQUEPTR(materialtexture, materials)
		node* get_rootnode() {return m_root.get();}
		void set_rootnode(node* node) { m_root.reset(node); }

		bool load(std::string path, int loadtype = aiProcess_Triangulate| aiProcess_FlipUVs );
		void pasernode(const aiScene* scene,node* node, aiNode* ainode);
		bool get_node_transform(IN int animaindex, IN double sec, IN node* node, OUT glm::mat4& mat);
		bool ergodic_node(MODEL::node* node,const std::function<bool(MODEL::node*)> &AccessFuntion);
	private:
		std::string m_name;
		std::vector<std::unique_ptr<mesh>> m_meshs;
		std::vector<std::unique_ptr<materialtexture>> m_materials;
		Assimp::Importer m_importer;
		std::unique_ptr<node> m_root;
	};






}