#include "font.h"
#include "fonterror.h"
#include <boost/log/trivial.hpp>
#include "util.h"

namespace GLUI
{
	std::unique_ptr<Font_Library> Font_Library::kFont_Library;

	Font_Library::~Font_Library()
	{
		if (is_available())
			FT_Done_FreeType(m_library);
	}

	bool Font_Library::init()
	{
		if (is_available())
			FT_Done_FreeType(m_library);

		m_is_available = false;

		//初始化全局的句柄
		int errorcode = FT_Init_FreeType(&m_library);
		if (!SUCCESS_FONT(errorcode)) {
			BOOST_LOG_TRIVIAL(error) << font_error_string(errorcode);
			m_is_available = false;
		}
		else
			m_is_available = true;
		return m_is_available;
	}


	FT_Library Font_Library::staticlib()
	{
		if (!kFont_Library.get())
			kFont_Library.reset(new Font_Library);
		if (!kFont_Library->is_available())
			kFont_Library->init();
		if (kFont_Library->is_available())
			return kFont_Library->lib();

		return nullptr;
	}


////////////////Font////////////////////////////////////
	MangerTemplateClassCPP(fontmanger)
	Font::~Font()
	{
		if(is_available())
			FT_Done_Face(m_face);
	}


	bool Font::init(const char* fontpath)
	{
		FT_Library flib = Font_Library::staticlib();
		if (flib == nullptr)
			return false;

		//初始化对应的文件的格式句柄
		int errorcode = FT_New_Face(flib, fontpath, 0, &m_face);
		if (!SUCCESS_FONT(errorcode)) {
			BOOST_LOG_TRIVIAL(error) << font_error_string(errorcode);
			m_is_available = false;
		}
		m_is_available = true;
		return m_is_available;
	}


	bool Font::set_italic(bool italic)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "set_italic  cannot set italic in unavailable font!";
			return false;
		}

		FT_Matrix m;	
		if (italic)
			m.xy = 0.2f * 0x10000L;
		else
			m.xy = 0;
		m.xx = 0x10000L;//0x10000L是16，尺度是16
		m.yx = 0;
		m.yy = 0x10000L;
		FT_Set_Transform(m_face, &m, NULL);
		set_statue(ITALIC);
		return true;
	}


	bool Font::set_font_size(int width, int height)
	{
		int errorcode = FT_Set_Pixel_Sizes(m_face, width, height);
		if (!SUCCESS_FONT(errorcode)) {
			BOOST_LOG_TRIVIAL(error) << font_error_string(errorcode);
			return false;
		}
		m_pixel_width = width;
		m_pixel_height = height;
		return true;
	}


	//MONO模式每1个像素仅用1bit保存，只有黑和白。
	//1个byte可以保存8个像素，1个int可以保存8*4个像素。
	void ConvertMONOToRGBA(FT_Bitmap *source, unsigned char *rgba)
	{
		unsigned char *s = source->buffer;
		unsigned char *t = rgba;

		for (unsigned char y = source->rows; y > 0; y--)
		{
			unsigned char *ss = s;
			unsigned char *tt = t;

			//因为这边的宽度是虚的，一个byte表示8个宽度，所以这里要除以8
			for (unsigned char x = source->width >> 3; x > 0; x--)
			{
				unsigned char val = *ss;

				for (unsigned char i = 8; i > 0; i--)
				{
					tt[0] = tt[1] = tt[2] = tt[3] = (val & (1 << (i - 1))) ? 0xFF : 0x00;
					tt += 4;
				}

				ss += 1;
			}

			int rem = source->width & 7;

			if (rem > 0)
			{
				int val = *ss;

				for (int x = rem; x > 0; x--)
				{
					tt[0] = tt[1] = tt[2] = tt[3] = (val & 0x80) ? 0xFF : 0x00;
					tt += 4;
					val <<= 1;
				}
			}
			s += source->pitch;
			t += source->width * 4;    //pitch
		}
	}

	//GRAY模式1个像素用1个字节保存。
	void ConvertGRAYToRGBA(FT_Bitmap *source, unsigned char *rgba)
	{
		for (unsigned int y = 0; y < source->rows; y++)
		{
			for (unsigned int x = 0; x < source->width; x++)
			{
				unsigned char *s = &source->buffer[(y * source->pitch) + x];
				unsigned char *t = &rgba[((y * source->pitch) + x) * 4];

				t[0] = t[1] = t[2] = *s;
				t[3] = *s;
			}
		}
	}


	bool Font::character_image(FT_ULong character, std::shared_ptr<image>& im)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "not available font!";
			return false;
		}

		FT_UInt glyph_index = FT_Get_Char_Index(m_face, character);
		if (character == glyph_index) {
			BOOST_LOG_TRIVIAL(error) << "character_image: cannot find character:"<< character;
			return false;
		}

		//找到向量图，存在face->glyph
		int errorcode = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
		if (!SUCCESS_FONT(errorcode)) {
			BOOST_LOG_TRIVIAL(error) << font_error_string(errorcode);
			return false;
		}

		//FT_GlyphSlot glyph = m_face->glyph;
		//FT_Render_Mode render_mode = m_antialias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO;
		errorcode = FT_Render_Glyph(m_face->glyph, render_mode);
		if (!SUCCESS_FONT(errorcode)) {
			BOOST_LOG_TRIVIAL(error) << font_error_string(errorcode);
			return false;
		}     
		FT_Bitmap bitmap = m_face->glyph->bitmap;
	 
		switch (bitmap.pixel_mode)
		{
		case FT_PIXEL_MODE_MONO:
			im->generateempty(8 * bitmap.width,bitmap.rows , 4);
			ConvertMONOToRGBA(&bitmap, im->m_data);
			break;
		case FT_PIXEL_MODE_GRAY:
			im->generateempty(bitmap.width,bitmap.rows , 4);
			ConvertGRAYToRGBA(&bitmap, im->m_data);
			break;
		case FT_PIXEL_MODE_BGRA:
			im->generateempty(bitmap.width, bitmap.rows, 4);
			memcpy(im->m_data, bitmap.buffer, bitmap.width*bitmap.rows * 4);
			break;
		default:
			BOOST_LOG_TRIVIAL(error) << "unsupport type";
			return false;
			break;
		}
		return true;

	}

	bool Font::DrowText(std::shared_ptr<Texture2D>& tex, int x, int y,wchar_t* str, int size, int maxwidth)
	{
		if (maxwidth == 0)  maxwidth = tex->Width;
	
		if (size == 0)
			size = wcslen(str);

		if (!tex.get()) {
			BOOST_LOG_TRIVIAL(error) << "DrowText:no texture";
			return false;
		}

		if (tex->Width < x || tex->Height < y) {
			BOOST_LOG_TRIVIAL(error) << "DrowText:out of texture size";
			return false;
		}

		if (tex->Format!= GL_RGBA){
			BOOST_LOG_TRIVIAL(error) << "DrowText:only support GL_RGBA texture now";
			return false;
		}

		int xpos = x;
		int ypos = y;
		std::shared_ptr<image> im(new image());
		int advancex = 0;
		int advancey = 0;
		for (int i = 0; i < size; i++)
		{
			if (str[i] == L'\n') {
				//y = y + im->m_height;
				ypos = ypos + m_pixel_height;
				if (ypos + m_pixel_height >= tex->Height)
					return true;
				xpos = x;
				continue;
			}
			if (str[i] == L' ') {
				xpos = (advancex!=0)? xpos + advancex: xpos + m_pixel_width;
				//xpos = xpos + m_pixel_width;
				continue;
			}
			if (!character_image(str[i], im))
				continue;

			//x = x + m_face->glyph->bitmap_left;
			advancex = m_face->glyph->advance.x >> 6;
			advancey = m_face->glyph->advance.y >> 6;
			//超出区域，忽略
		
			
			if (xpos + im->m_width >= maxwidth) {//tex->Width
				//如果自动换行
				if (m_autochangeline) {
					ypos = ypos + m_pixel_height;
					if (ypos + m_pixel_height >= tex->Height)
						return true;
					xpos = x;
				}
				else
				  continue;
			}
			
			

			int yyy = ypos + m_pixel_height -  m_face->glyph->bitmap_top; // y - (m_face->glyph->bitmap.rows - m_face->glyph->bitmap_top);
			if (!tex->Copy(0, (char*)im->m_data, xpos, yyy, im->m_width, im->m_height))
				continue;
			//xpos = xpos + m_pixel_width;
			if (advancex <= im->m_width)
				xpos = xpos + advancex;
			else
				xpos = xpos + im->m_width;
		}
		return true;

	}

	
	//FT_GlyphSlot  slot;
	//FT_Matrix     matrix;              /* transformation matrix */
	//FT_UInt       glyph_index;
	//FT_Vector     pen;                 /* untransformed origin */
	//int           n;


	//... initialize library ...
	//	... create face object ...
	//	... set character size ...

	//	slot = face->glyph;                /* a small shortcut */

	//									   /* set up matrix */
	//matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	//matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	//matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	//matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	///* the pen position in 26.6 cartesian space coordinates */
	///* start at (300,200)                                   */
	//pen.x = 300 * 64;
	//pen.y = (my_target_height - 200) * 64;

	//for (n = 0; n < num_chars; n++)
	//{
	//	/* set transformation */
	//	FT_Set_Transform(face, &matrix, &pen);

	//	/* load glyph image into the slot (erase previous one) */
	//	error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
	//	if (error)
	//		continue;  /* ignore errors */

	//				   /* now, draw to our target surface (convert position) */
	//	my_draw_bitmap(&slot->bitmap,
	//		slot->bitmap_left,
	//		my_target_height - slot->bitmap_top);

	//	/* increment pen position */
	//	pen.x += slot->advance.x;
	//	pen.y += slot->advance.y;
	//}
	
}