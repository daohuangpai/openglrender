#include "imageview.h"
#include <boost/log/trivial.hpp>
#include "windows.h"
/*
BOOST_LOG_TRIVIAL(trace)<<"A trace severity message";
BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
BOOST_LOG_TRIVIAL(info) << "An informational severity message";
BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
BOOST_LOG_TRIVIAL(error) << "An error severity message";
BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
*/
namespace GLUI {


bool imageview::setimage(const char* imagename)
{
	m_mane = imagename;
	if (!textur2demanger::sharetextur2demanger()->get(std::string(imagename), m_UI_Texture))
	{
		m_UI_Texture.reset(Texture2D::CreateTexture2D(imagename));
		if (!m_UI_Texture.get() || !m_UI_Texture->is_available())
			return false;
		textur2demanger::sharetextur2demanger()->set(std::string(imagename), m_UI_Texture);
	}
	
	windows* w = GetWindows();
	if (w)
		w->Repaint(nullptr);
	return true;
}


void imageview::OnDrow(rect* rect)
{
	if (!m_UI_Texture.get() || !m_UI_Texture->is_available()) {
		BOOST_LOG_TRIVIAL(error) << "no Available Texture"<< m_mane.data();
		return;
	}
	GLUI::rect rrect = *rect;
	RelativeP2WorldP(rrect.x, rrect.y);
	WorldP2OpenglP(rrect);
	glViewport(rrect.x, rrect.y, rrect.width, rrect.height);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_UI_Texture->ID());
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f); 
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

}

void imageview::OnDrop(int count, const char** paths)
{
	setimage(paths[0]);
}














}