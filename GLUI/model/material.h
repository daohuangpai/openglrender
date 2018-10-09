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
			//�������
			DIFFUSE = 1,
			//���淴���
			SPECULAR ,
			//������
			AMBIENT ,
			//������
			EMISSIVE ,
			//�߶���Ϣ����
			HEIGHT ,
			//��������
			NORMALS ,
			//����������
			SHININESS ,
			//��͸����ͼƬ
			OPACITY ,
			//�û���ͼ
			DISPLACEMENT ,
			//�決��ͼ
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
