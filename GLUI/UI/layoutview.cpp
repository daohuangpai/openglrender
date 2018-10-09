#include "layoutview.h"

namespace GLUI
{
 
 

	void Hlayout::OnSize()
	{
		if (!m_parent)
			return;

		std::vector<view*>& sons = m_son_view;
		//仅仅当父节点只有这一个布局子节点时，数据才随之变化
		if (m_parent->sons().size() == 1) {
			m_rect.x = 0;
			m_rect.y = 0;
			m_rect.width = m_parent->get_rect().width;
			m_rect.height = m_parent->get_rect().height;
		}
		int widtheach = m_rect.width / sons.size();
		int begin = 0;
		for(std::vector<view*>::iterator itr = sons.begin();itr!= sons.end();itr++)
		{
			(*itr)->set_rect(begin, 0, widtheach, (*itr)->get_size().height);
			begin += widtheach;
		}
	}

	void Vlayout::OnSize()
	{
		if (!m_parent)
			return;

		std::vector<view*>& sons = m_son_view;
		//仅仅当父节点只有这一个布局子节点时，数据才随之变化
		if (m_parent->sons().size() == 1) {
			m_rect.x = 0;
			m_rect.y = 0;
			m_rect.width = m_parent->get_rect().width;
			m_rect.height = m_parent->get_rect().height;
		}
		int heighteach = m_rect.height / sons.size();
		int begin = 0;
	 
		for (std::vector<view*>::iterator itr = sons.begin(); itr != sons.end(); itr++)
		{
			(*itr)->set_rect(0, begin, (*itr)->get_size().width, heighteach);
			begin += heighteach;
		}
	}




}