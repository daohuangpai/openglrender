#include "view.h"
#include <algorithm>//注意要包含该头文件
#include <boost/log/trivial.hpp>
#include "windows.h"

namespace GLUI
{



	view::~view()
	{
		for (std::vector<view*>::iterator itr = m_son_view.begin();
			itr != m_son_view.end(); itr++)
		{
			delete *itr;
		}
		m_son_view.clear();
	}

	void view::insert_view(view* object)
	{
		std::vector<view*>::iterator itr = std::find(m_son_view.begin(),
                                          m_son_view.end(), object);
		if (itr == m_son_view.end()) {
			m_son_view.push_back(object);
			object->m_parent = this;
		}
	}
	void view::remove_view(view* object)
	{
		std::vector<view*>::iterator itr = std::find(m_son_view.begin(), m_son_view.end(), object);
		if (itr == m_son_view.end()) {
			object->m_parent = NULL;
			m_son_view.erase(itr);
		}
	}



	void view::Drow(rect parent_drow_rect)
	{
		//以父窗体的绘图区域作为子区域的最大绘图区间
		if (is_statue(IS_HIDE)){
			return;
		}

		rect drowrect = get_size();
		if (m_parent){
			//得到父的区域，但是转化为本view的坐标系
			rect pr = parent_drow_rect;
			pr.x = 0;
			pr.y = 0;
			//取得and的区域
			drowrect = rect_operation_and(get_size(), pr);
		}

		if(rect_available(drowrect))
		     OnDrow(&drowrect);

		for (std::vector<view*>::iterator itr = m_son_view.begin();
			itr != m_son_view.end(); itr++)
		{
			 (*itr)->Drow(drowrect);
		}
	}



	/*bool view::HitTest(int x, int y)
	{ 
		if (is_statue(IS_HIDE)) {
			return false;
		}

		return ((x >= m_rect.x)
			&& (x <= m_rect.x + m_rect.width)
			&& (y >= m_rect.y)
			&& (y <= m_rect.y + m_rect.height));
	}*/

	bool view::HitTest(int x, int y)
	{
		if (is_statue(IS_HIDE)) {
			return false;
		}

		bool result =  ((x >= m_rect.x)
			&& (x <= m_rect.x + m_rect.width)
			&& (y >= m_rect.y)
			&& (y <= m_rect.y + m_rect.height));

		//BOOST_LOG_TRIVIAL(info) << "h?"<< result << "HitT:"<<x<<","<<y<<"windows:"<< m_rect.x<<","<< m_rect.y<< "," << m_rect.x + m_rect.width<< "," << m_rect.y + m_rect.height<<",";


		return result;
	}

	rect view::get_real_size()
	{
		rect r = get_size();
		RelativeP2WorldP(r.x,r.y);
		return r;
	}


	void view::WorldP2RelativeP(int& x, int &y)
	{
		view* parent = m_parent;
		while (parent)
		{
			const rect& prect = parent->get_size();
			x -= prect.x;
			y -= prect.y;
			parent = parent->m_parent;
		}
	}

	void view::RelativeP2WorldP(int& x, int &y)
	{
		view* parent = m_parent;
		while (parent)
		{
			const rect& prect = parent->get_size();
			x += prect.x;
			y += prect.y;
			parent = parent->m_parent;
		}
	}

	void view::WorldP2OpenglP(GLUI::rect& rect)
	{
		view* root = this;
		while (root->m_parent)
		{
			root = root->m_parent;
		}
		const GLUI::rect& prect = root->get_size();
		rect.y = prect.height - rect.height- rect.y;
	}


	void view::repaint()
	{
		windows* w = GetWindows();
		if (w)
			w->Repaint(nullptr);
	}

	windows* view::GetWindows()
	{
		view* root = this;
		while (root)
		{
			windows* w = root->Windows();
			if (w)
				return w;
			root = root->m_parent;
		}
		return nullptr;
	}

	void view::sizechange()
	{
		OnSize();
		std::vector<view*>& sons = m_son_view;
		for each(view* v in sons)
		{
			v->OnSize();
		}
	}

}
