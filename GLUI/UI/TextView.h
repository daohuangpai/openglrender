#pragma once
#include "../OGL/font.h"
#include "view.h"

namespace GLUI
{


	class textview : public view {
	public:
		virtual void OnSize() override;
		virtual void OnDrow(rect* rect)override;
		void setfontsize(int size);
		bool init(char* fontname);
		void settext(std::wstring& text);
		bool is_available();
	private:
		std::shared_ptr<Font> m_Font;
		std::shared_ptr<Texture2D> m_tex;
		std::wstring m_text;
	};

}