#pragma once
#include "../OGL/shader.h"
#include "../OGL/texture.h"

namespace MODEL
{

	class materialtexture
	{
	public:
		enum MATERIAL_DETAIL_TYPE
		{
			//漫反射光
			DIFFUSE = 1,
			//镜面反射光
			SPECULAR ,
			//环境光
			AMBIENT ,
			//发光体
			EMISSIVE ,
			//高度信息纹理
			HEIGHT ,
			//法线纹理
			NORMALS ,
			//反射光的亮度
			SHININESS ,
			//不透明度图片
			OPACITY ,
			//置换贴图
			DISPLACEMENT ,
			//烘焙贴图
			LIGHTMAP ,
			REFLECTION ,
			MATERIAL_DETAIL_TYPE_SIZE
		};
		bool init(std::map<MATERIAL_DETAIL_TYPE, std::string>& files);

		GLUI::Texture2D* get_texture(MATERIAL_DETAIL_TYPE type,int index)
		{
			if (m_texturearray[type].size() <= index)
				return nullptr;
			return m_texturearray[type][index].get();
		}

		struct config
		{
			MATERIAL_DETAIL_TYPE type;
			unsigned int textureindex;
			unsigned int activenum;
		};

	private:	
		std::vector<std::shared_ptr<GLUI::Texture2D>>  m_texturearray[MATERIAL_DETAIL_TYPE_SIZE];
	};



}
