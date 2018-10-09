#include "ButtonViews.h"
#include <boost/log/trivial.hpp>

namespace GLUI {

	bool buttonimageview::setimage(BUTTON_TYPE type, const char* imagename)
	{
		std::string* str = NULL;
		switch (type)
		{
		case BUTTON_NOMAL:
			str = &m_Nomal;
			m_mane = imagename;
			imageview::setimage(imagename);
			break;
		case BUTTON_HOVEL:
			str = &m_Hovel;
			break;
		case BUTTON_HOT:
			str = &m_Hot;
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "buttonimageview::setimage:  undefine button type";
			return false;
			break;
		}
		*str = imagename;
		return true;
	}

	
	 void buttonimageview::OnEnter()
	 {
		 m_type = BUTTON_HOVEL;
		 imageview::setimage(m_Hovel.data());
	 }
	 void buttonimageview::OnLeave()
	 {
		 m_type = BUTTON_NOMAL;

		 imageview::setimage(m_Nomal.data());
	 }
	 void buttonimageview::OnLBclick()
	 {
		 m_type = BUTTON_HOVEL;
		 imageview::setimage(m_Hovel.data());
		 click(this);
	 }

	 void buttonimageview::Focus()
	 {
		 m_type = BUTTON_HOT;
		 imageview::setimage(m_Hot.data());
		 view::Focus();
	 }




}