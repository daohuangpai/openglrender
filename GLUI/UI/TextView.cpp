#include "TextView.h"
#include "../tool/path.h"
#include "windows.h"

namespace GLUI
{

	bool textview::is_available()
	{
		return (m_Font.get() && m_Font->is_available() && m_tex.get() && m_tex->is_available() && m_text.size() > 0);
	}

	bool textview::init(char* fontname)
	{
		if (!fontmanger::sharefontmanger()->get(std::string(fontname), m_Font))
		{
			m_Font.reset(new Font);
			if (!m_Font->init((PATH::fontpath() + std::string(fontname)).data())) {
				return false;
			}
			fontmanger::sharefontmanger()->set(std::string(fontname), m_Font);
		}

		if (!m_tex.get()|| !m_tex->is_available()) {
			m_tex.reset(new Texture2D);
			m_tex->Mipmapping = false;
			m_tex->Width = 640;
			m_tex->Height = 480;
			if (!m_tex->bind_data(NULL)) {
				return false;
			}
		}
		return true;
	}

	void textview::OnSize()
	{
		if (!is_available())
			return;
		m_Font->DrowText(m_tex, 0, 0, &m_text[0], m_text.size(), m_rect.width);
	}

	void textview::setfontsize(int size)
	{
	if(m_Font.get() && m_Font->is_available())
		m_Font->set_font_size(size, size);
	}

	void textview::OnDrow(rect* rect)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "no Available textview";
			return;
		}
		GLUI::rect rrect = *rect;
		RelativeP2WorldP(rrect.x, rrect.y);
		WorldP2OpenglP(rrect);
		glViewport(rrect.x, rrect.y, rrect.width, rrect.height);

		float fwidth = (float)m_rect.width/ (float)m_tex->Width;
		float fheight = (float)m_rect.height / (float)m_tex->Height;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_tex->ID());
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void textview::settext(std::wstring& text)
	{
		m_text = text;
		m_Font->DrowText(m_tex, 0, 0, &m_text[0], m_text.size());
		windows* w = GetWindows();
		if (w) w->Repaint(nullptr);
		return;
	}

}