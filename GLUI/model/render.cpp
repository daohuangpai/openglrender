#include "render.h"
#include<boost/timer.hpp> 
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
namespace MODEL
{

#define MESH_VAO_BIND(vao,arraybuffer,location)\
       if (arraybuffer&&arraybuffer->is_available()) {\
              vao.enable_vertex(location);\
	          vao.bindvectexbuffer(arraybuffer->buffer(), 0, location,\
		static_cast<GLint>(arraybuffer->elementsize() / sizeof(float)), GL_FLOAT, GL_FALSE, arraybuffer->bufferbegin());\
	   }

#define MESH_VAO_UNBIND(vao,arraybuffer,location)\
       if (arraybuffer&&arraybuffer->is_available()) {\
              vao.disabl_vertex(location);\
	   }

	void meshrender::global_setting(model* ml)
	{
		dataarray<materialtexture::config> a;
		m_nodeconfig[0].materialconfig.init(1);
		m_nodeconfig[0].meshconfig.init(1);
		m_nodeconfig[0].meshshaderconfig.init(1);
		m_nodeconfig[0].materialconfig[0] = a;
		m_nodeconfig[0].materialconfig.m_data[0].init(materialtexture::MATERIAL_DETAIL_TYPE_SIZE-1);
		auto b = m_nodeconfig[0].materialconfig[0].m_data;
		m_nodeconfig[0].materialconfig[0].m_data[0];
		for (int i = materialtexture::DIFFUSE - 1; i < materialtexture::MATERIAL_DETAIL_TYPE_SIZE - 1 ; i++)
		{
			m_nodeconfig[0].materialconfig[0].m_data[i].type = materialtexture::MATERIAL_DETAIL_TYPE(i + 1);
			m_nodeconfig[0].materialconfig[0].m_data[i].textureindex = 0;
			m_nodeconfig[0].materialconfig[0].m_data[i].activenum = i;
		}

		m_nodeconfig[0].meshconfig[0].colorindex = 0;
		m_nodeconfig[0].meshconfig[0].texturecoordsindex = 0;
		

		m_nodeconfig[0].meshshaderconfig[0].vectex_location = 0;
		m_nodeconfig[0].meshshaderconfig[0].texturecoords_location = 2;
		m_nodeconfig[0].meshshaderconfig[0].normal_location = 1;
		m_nodeconfig[0].meshshaderconfig[0].bitangents_location = 3;
		m_nodeconfig[0].meshshaderconfig[0].tangents_location = 4;
		m_nodeconfig[0].meshshaderconfig[0].color_location = 5;
		m_nodeconfig[0].meshshaderconfig[0].boneid_location = 6;
	}

	bool meshrender::meshdrow(mesh* mesh, mesh::config& config, shaderconfig& shaderconfig)
	{
		mesh::meshparam meshparam;
		mesh::buffer_vector& elements = mesh->get_elements_table();
		meshparam.color = mesh->get_color(config.colorindex);
		meshparam.texturecoords = mesh->get_texturecoords(config.texturecoordsindex);

		meshparam.vectex = &mesh->get_vectex();
		meshparam.normal = &mesh->get_normal();
		meshparam.bitangents = &mesh->get_bitangents();
		meshparam.tangents = &mesh->get_tangents();

		m_vao.bind();
		MESH_VAO_BIND(m_vao, meshparam.vectex, shaderconfig.vectex_location)
		MESH_VAO_BIND(m_vao, meshparam.normal, shaderconfig.normal_location)
		MESH_VAO_BIND(m_vao, meshparam.bitangents, shaderconfig.bitangents_location)
		MESH_VAO_BIND(m_vao, meshparam.tangents, shaderconfig.tangents_location)
		MESH_VAO_BIND(m_vao, meshparam.color, shaderconfig.color_location)
		MESH_VAO_BIND(m_vao, meshparam.texturecoords, shaderconfig.texturecoords_location)
		if(shaderconfig.anim&&mesh->get_bonetablebuffer().size()>0){
			m_vao.enable_vertex(shaderconfig.boneid_location);
			m_vao.bindvectexbuffer(mesh->get_bonetableID().buffer(), 0, shaderconfig.boneid_location,
						static_cast<GLint>(mesh->get_bonetableID().elementsize() / sizeof(int)), GL_INT, GL_FALSE, mesh->get_bonetableID().bufferbegin());
			mesh->get_bone_mats().buffer()->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER, 0, 0, mesh->get_bone_mats().bytesize());
			mesh->get_bonetablebuffer().buffer()->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER, 1, 0, mesh->get_bonetablebuffer().bytesize());
		}
		//	boost::log::add_file_log("sample.log");
		//meshparam.vectex->AccessData([](glm::vec3* data,int size)->bool
		//{
		//	for (int i = 0; i < size; i++)
		//	{
		//		//printf("%f,%f,%f,\r\n", data[i][0], data[i][1], data[i][2]);
		//		BOOST_LOG_TRIVIAL(info) << data[i][0] << "," << data[i][1] << "," << data[i][2];
		//	}
		//	return true;
		//}
		//);

		//BOOST_LOG_TRIVIAL(info) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
		//((SimpleBufferArray<glm::vec2>*)meshparam.texturecoords)->AccessData([](glm::vec2* data, int size)->bool
		//{
		//	for (int i = 0; i < size; i++)
		//	{
		//		// printf("%f,%f,\r\n", data[i][0], data[i][1]);
		//		BOOST_LOG_TRIVIAL(info) << data[i][0] << "," << data[i][1];
		//	}
		//	return true;
		//}
		//);
		//BOOST_LOG_TRIVIAL(info) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
		//elements[0]->buffer()->AccessData([](char* data, int size)
		//{
		//	unsigned int* ptr = (unsigned int*)data;
		//	int length = size / sizeof(unsigned int);
		//	for (int i = 0; i < length; i++)
		//	{
		//		printf("%d,\r\n", ptr[i]);
		//	}
		//}
		//);
		if (elements.size() > 0)
		{
			if (!m_vao.Drawelements(elements[0]->buffer().get(), mesh->get_drowtype(),
					elements[0]->buffer()->getsize() / sizeof(GLint), Vao::UNSIGNED_INT, (GLvoid*)elements[0]->bufferbegin()))
					return false;
		}
	/*	for(int i = 0;i<elements.size();i++)
		{
			Vao::DROW_ELEMENT_DATA_TYPE elementdatatype;
			switch (elements[i]->elementsize())
			{
			case sizeof(GLbyte) :
				elementdatatype = Vao::UNSIGNED_BYTE;
				break;
			case sizeof(GLshort) :
				elementdatatype = Vao::UNSIGNED_SHORT;
				break;
			case sizeof(GLint) :
				elementdatatype = Vao::UNSIGNED_INT;
				break;
			default:
			  {
				BOOST_LOG_TRIVIAL(error) << "meshrender::meshdrow: element type error "; 
				return false; 
			  }
			}

			if (!m_vao.Drawelements(elements[i]->buffer().get(), mesh->get_drowtype(),
				elements[i]->size(), elementdatatype, (GLvoid*)elements[i]->bufferbegin()))
				return false;
		}*/
		if(elements.size()==0)
		{
			return m_vao.DrowArray(mesh->get_drowtype(), 0, meshparam.vectex->size());
		}
		MESH_VAO_UNBIND(m_vao, meshparam.vectex, shaderconfig.vectex_location)
		MESH_VAO_UNBIND(m_vao, meshparam.normal, shaderconfig.normal_location)
		MESH_VAO_UNBIND(m_vao, meshparam.bitangents, shaderconfig.bitangents_location)
		MESH_VAO_UNBIND(m_vao, meshparam.tangents, shaderconfig.tangents_location)
		MESH_VAO_UNBIND(m_vao, meshparam.color, shaderconfig.color_location)
		MESH_VAO_UNBIND(m_vao, meshparam.texturecoords, shaderconfig.texturecoords_location)
		if (shaderconfig.anim&&mesh->get_bonetablebuffer().size()>0) {
			m_vao.disabl_vertex(shaderconfig.boneid_location);
		}
		return true;
	}


	bool meshrender::materialtexturebind(materialtexture* material, dataarray<materialtexture::config>& config)
	{
		int size = config.get_size();
		for (int i = 0; i < size; i++)
		{

		  GLUI::Texture2D* tex = material->get_texture(config.m_data[i].type, config.m_data[i].textureindex);
		  if((tex!=nullptr)&&tex->is_available())
		  {
			  tex->Active(config.m_data[i].activenum);
			  tex->bind();
		  }
		}
		return true;
	}



	bool meshrender::drow(model* ml, glm::mat4 tranf,std::shared_ptr<GLUI::shader>& shader, char* transfranname,MODEL::node* node, int animaindex, double sec)
	{
		std::vector<node::param>& nodeparams = node->get_param();
		bool useglobalsetting = false;
		nodeconfig* settingptr = nullptr;
		auto nodeconfigitr = m_nodeconfig.find(node);
		if (nodeconfigitr != m_nodeconfig.end()) {
			settingptr = &nodeconfigitr->second;
			useglobalsetting = false;
		}
		else {
			if (m_nodeconfig.find(0) == m_nodeconfig.end())
				global_setting(ml);
			settingptr = &m_nodeconfig[0];
			useglobalsetting = true;
		}
		nodeconfig& setting = *settingptr;
		glm::mat4 tran;
		//ÓÐ¶¯»­
		if (animaindex >= 0) {
			glm::mat4 mat;
			ml->get_node_transform(animaindex, sec, node,mat);
			tran = tranf*mat;
		/*if (1)
			{
				glm::mat4 m = node->get_transformation();
				debug_Mat_info(m)
				debug_Mat_info(mat)
			}*/
			
		}
		else {
			tran = tranf*node->get_transformation();
		}
	 //  if (1)
		//{
		//debug_Mat_info(tranf)
		//debug_Mat_info(tran)
		//}
		/*glm::mat4 p = glm::mat4(300.0);
		float d[] = { 3.0f,0.0f,0.0f,0.0f,
			          0.0f,9.0f,0.0f,0.0f,
			          0.0f,0.0f,3.0f,0.0f,
			          0.0f,0.0f,0.0f,3.0};
		for(int i = 0;i<16;i++)
		{
			d[i] = fmod(sec*d[i], 0.03f);
		}*/
		//tran = glm::make_mat4<float>(d)*tran;
		shader->set_uniform_mat_4_4(transfranname,(GLfloat*)&tran[0]);

		int i = 0;
		for (auto itr = nodeparams.begin(); itr != nodeparams.end(); itr++)
		{
			mesh* meshs = ml->get_meshs(itr->meshindex);
			materialtexture* materals = ml->get_materials(itr->materialindex);
			if (materals != nullptr)
			{
				dataarray<materialtexture::config>& materialconfig = useglobalsetting ? setting.materialconfig.m_data[0] : setting.materialconfig.m_data[i];
				if(!materialtexturebind((materialtexture*)(materals), materialconfig))
					return false;
			}

			if (meshs != nullptr) 
			{
				mesh::config& config = useglobalsetting ? setting.meshconfig.m_data[0] : setting.meshconfig.m_data[i];
				shaderconfig& shaderconfig = useglobalsetting ? setting.meshshaderconfig.m_data[0] : setting.meshshaderconfig.m_data[i];
				shaderconfig.anim = (animaindex >= 0);
				if(!meshdrow((mesh*)(meshs), config, shaderconfig))
			        return false;
			}	
		   i++;
    	}	

		std::vector<std::unique_ptr<MODEL::node>>& childs = node->childs();
		for (auto itr = childs.begin(); itr != childs.end(); itr++)
		{
			if (!drow(ml, tran, shader, transfranname, itr->get(), animaindex,sec))
				return false;
		}
		return true;
	 }
}