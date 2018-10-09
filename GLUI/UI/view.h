#pragma once
#include "util.h"
#include <vector>
#include <string>
#include "event.h"
#include "../common/Signal.h"

namespace GLUI {

class RootView;
class windows;

class view : public SendObject, public ReceiveObject
	{
	public:
		virtual~view();

		enum UI_TYPE {
			ROOT_VIEW = 0,
			TEXTURE_VIEW,
			IMAGE_VIEW,
			BUTTON_IMAGE_VIEW,
			VIEW,
			HLAYOUT,
			VLAYOUT,
			FILLLAYOUT

		};
		virtual UI_TYPE type() { return VIEW; }
		void insert_view(view* object);
		void remove_view(view* object);
		rect& get_size() { return m_rect; }
		rect get_real_size();//得到真实坐标的区域;
	
		void WorldP2RelativeP(int& x,int &y);//世界坐标到这个view的相对坐标;
		void RelativeP2WorldP(int& x, int &y);//这个view的相对坐标到世界坐标
		void WorldP2OpenglP(GLUI::rect& rect);

	public:
		std::vector<view*>& sons() { return m_son_view; }
		rect get_rect() { return m_rect; }
		virtual void set_rect(rect& rect) { m_rect = rect; sizechange();}
		virtual void set_rect(int x, int y, int w, int h) {m_rect.x = x; m_rect.y = y; m_rect.width = w; m_rect.height = h; sizechange();}
		virtual void Drow(rect parent_drow_rect);
		void sizechange();
		windows* GetWindows();
		virtual windows* Windows() { return nullptr; }
		void repaint();
	protected:
		virtual bool HitTest(int x, int y);//检测点击是否区域内
		
	protected:
		//系统消息处理后的函数
		virtual void OnDrow(rect* rect) {}
		virtual void OnMouseMove(int x, int y) {}
		virtual void OnEnter() {}
		virtual void OnSize() {}
		virtual void OnLeave() {}
		virtual void OnFocus() {}
		virtual void OnLostFocus() {}
		virtual void OnLBclick() {}
		virtual void OnRBclick() {}
		virtual void OnMBclick() {}
		virtual void OnKey(int keycode, event::key_statue statue) {}//按键消息
		virtual void OnInput(const char* value) {}//输入的单个字符，可以是汉字,已转UTF8;
		virtual void OnDrop(int count, const char** paths) {}//输入拖入的文件路径，数目，路径数组;
		virtual void OnScroll(int y) {}
	protected:
		virtual void Enter()
		{
			if (!is_statue(IS_ENTER)) {
				set_statue(IS_ENTER);
				OnEnter();
			}
		}
		virtual void Leave()
		{
			if (is_statue(IS_ENTER)) {
				clear_statue(IS_ENTER);
				OnLeave();
			}
		}
		virtual void Focus()
		{
			if (!is_statue(IS_ACTIVE)) {
				set_statue(IS_ACTIVE);
				OnFocus();
			}
		}
		virtual void LostFocus()
		{
			if (is_statue(IS_ACTIVE)) {
				clear_statue(IS_ACTIVE);
				OnLostFocus();
			}
		}

		 inline bool is_hide() { return is_statue(IS_HIDE); }
		 inline void show() { clear_statue(IS_HIDE); }
		 inline void hide() { set_statue(IS_HIDE); }
	protected:
		rect m_rect;
		std::vector<view*> m_son_view;
		view* m_parent = nullptr;
	protected:
		//about msg 
		enum {
			IS_ENTER = 0x0001,
			IS_ACTIVE = 0x0002,
			IS_LB_DOWN = 0x0004,
			IS_RB_DOWN = 0x0008,
			IS_MB_DOWN = 0x0010,
			IS_HIDE = 0x0020
		};
		int m_statue = 0;
		

	public:
		inline bool is_statue(int Checkstatue)
		{
			return (m_statue&Checkstatue)!=0;
		}
		inline void close_statue()
		{
			m_statue = 0;
		}
		inline void set_statue(int Checkstatue)
		{
			m_statue = m_statue|Checkstatue;
		}
		inline void clear_statue(int Checkstatue)
		{
			m_statue = m_statue&(~Checkstatue);
		}
		 friend class RootView;
	};





}//namespace