#pragma once

#include "view.h"
namespace GLUI {

class GLwindow;
class RootView : public view
{
public:
	virtual UI_TYPE type() {return ROOT_VIEW;}
private:
	static view* GetHitView(view* root, int x, int y);
	static view* HitView(view* root, int x, int y);
	static view* GetEnterView(view* root);
protected://系统消息处理函数
	virtual void MouseMove(int x, int y);
	virtual void Mouse(event::mouse_event event, event::mouse_statue statue);
	virtual void Key(int keycode, event::key_statue statue);
	virtual void Input(const char* value);
	virtual void Drop(int count, const char** paths);
	virtual void Scroll(int y);
	virtual void WindowsLostFouce() { if (m_activeview) m_activeview->LostFocus(); m_activeview = nullptr; }

	virtual windows* Windows() override { return m_window; }
private:
	virtual bool HitTest(int x, int y) { return true; }
private:
	view* m_enterview = nullptr;
	view* m_activeview = nullptr;
	windows* m_window = nullptr;
	friend class GLwindow;

};



}