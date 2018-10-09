#pragma once
#include <ft2build.h>  
#include FT_FREETYPE_H  
#include <memory>
#include "image.h"
#include "texture.h"
#include "mangertemplate.h"

namespace GLUI
{

class Font_Library {
public:
	virtual ~Font_Library();
	bool  is_available() { return m_is_available; }
	bool init();
	FT_Library lib() { return m_library; }
	static FT_Library staticlib();
private:
	FT_Library m_library; /* 库的句柄 */
	bool m_is_available = false;
	static std::unique_ptr<Font_Library> kFont_Library;
};



class Font {
public:
	enum FontType{
		ITALIC = 0x01,//斜体
	};
	virtual ~Font();
	bool is_available() { return m_is_available; }
	bool init(const char* fontpath);
	bool set_italic(bool italic);
	bool set_font_size(int width, int height);
	bool is_statue(FontType statue) { return m_statue&statue; }
	bool character_image(FT_ULong character,std::shared_ptr<image>& im);
	bool DrowText(std::shared_ptr<Texture2D>& tex,int x,int y,wchar_t* str,int size,int maxwidth = 0);
private:
	inline void set_statue(FontType statue) { m_statue = m_statue|statue; }
private:
	FT_Face m_face; /* face对象的句柄 */
	bool m_is_available = false;
	FT_Render_Mode render_mode = FT_RENDER_MODE_NORMAL;//if FT_RENDER_MODE_MONO will product 2-color image
	int m_statue = 0;
	int m_pixel_width = 0;
	int m_pixel_height = 0;
	bool m_autochangeline = true;
};

MangerTemplateClassPtr(std::string, Font, fontmanger)




}