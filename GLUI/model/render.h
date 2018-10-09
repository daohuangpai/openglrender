#pragma once
#include "mesh.h"
#include "material.h"
#include "../OGL/shader.h"
#include "../common/configure.h"
#include "../OGL/vao.h"
#include "node.h"
#include "model.h"
#include "../common/glslvalue.h"
#include "../OGL/shader.h"
#include "dataarray.h"

namespace MODEL
{

	class meshrender {
	public:
		struct shaderconfig
		{
			int vectex_location;
			int normal_location;
			int texturecoords_location;
			int bitangents_location;
			int tangents_location;
			int color_location;
			int boneid_location;
			bool anim;
		};
    	struct nodeconfig
		{
			dataarray<mesh::config> meshconfig;
			dataarray<shaderconfig> meshshaderconfig;
			dataarray<dataarray<materialtexture::config>> materialconfig;
		};
		
		void init() { m_vao.generate(); }
		void global_setting(model* ml);
		virtual bool drow(model* ml, glm::mat4 tranf, std::shared_ptr<GLUI::shader>& shader,char* transfranname,MODEL::node* node,int animaindex = -1,double sec = 0.0);
		bool meshdrow(mesh* mesh, mesh::config& config, shaderconfig& shaderconfig);
		bool materialtexturebind(materialtexture* material, dataarray<materialtexture::config>& config);
	private:
		std::map<node*, nodeconfig> m_nodeconfig;
		GLUI::Vaodynamic m_vao;
	};
}

