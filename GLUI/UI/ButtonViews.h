#pragma once
#include "imageview.h"

namespace GLUI {

	class buttonimageview : public imageview {
	public:
		enum BUTTON_TYPE{
			BUTTON_NOMAL,
			BUTTON_HOVEL,
			BUTTON_HOT
		};
		bool setimage(BUTTON_TYPE type,const char* imagename);
		virtual UI_TYPE type() { return BUTTON_IMAGE_VIEW; }
		
		//slot
		void click(buttonimageview* view){Run(&buttonimageview::click,std::move(view));}
	protected:
		virtual void OnEnter() override;
		virtual void OnLeave() override;
		virtual void OnLBclick() override;
		virtual void Focus()override;
		virtual void OnDrop(int count, const char** paths) override {}
	private:
		std::string m_Nomal;
		std::string m_Hovel;
		std::string m_Hot;
		BUTTON_TYPE m_type = BUTTON_NOMAL;
	};


}