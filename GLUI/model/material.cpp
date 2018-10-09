#include "material.h"

namespace MODEL
{

	bool materialtexture::init(std::map<MATERIAL_DETAIL_TYPE, std::string>& files)
	{
		for (auto pm = files.begin(); pm != files.end(); pm++)
		{
			std::shared_ptr<GLUI::Texture2D> tex;
			tex.reset(GLUI::Texture2D::CreateTexture2D(pm->second.data()));
			if (tex.get()) {
				m_texturearray[pm->first].push_back(std::move(tex));
			}
			else {
				BOOST_LOG_TRIVIAL(error) << "materialtexture::init: load file failed ,path:" << pm->second.data();
				return false;
			}
		}
		return true;
	}






}