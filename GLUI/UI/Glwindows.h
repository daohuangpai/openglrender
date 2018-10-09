#pragma once
#include "glfwindows.h"
#include "RootView.h"
namespace GLUI
{



	class GLwindow :public windows {
	public:
		//virtual void event(GLUI::event*; event) override;//消息处理
		virtual void OnMove(int x, int y) override;
		virtual void OnSize(int w, int h) override;
		virtual bool OnClose() override;//返回false禁止关闭
		virtual void OnPaint() override;//刷新图像
		virtual void OnFocus() override;
		virtual void OnLostFocus() override;
		virtual void OnMinimized() override;
		virtual void OnRestored() override;
		virtual void OnMouseMove(int x, int y) override;
		virtual void OnMouse(event::mouse_event event, event::mouse_statue statue) override;
		virtual void OnEnter() override;//鼠标进入窗口
		virtual void OnLeave() override;//鼠标离开窗口
		virtual void OnScroll(int y) override;//鼠标滚动事件，+1向上滚，-1向下滚
		virtual void OnKey(int keycode, event::key_statue statue) override;
		virtual void OnInput(const char* value)override;//输入的单个字符，可以是汉字,已转UTF8;
		virtual void OnDrop(int count, const char** paths) override;//输入拖入的文件路径，数目，路径数组;

	    void CreateWindows(int width, int height, char* title);
		RootView* rootview() { return m_rootviews.get(); }
		GLFWwindow* windows() { return m_window; }
		virtual void Repaint(rect* rect);
	private:
		GLFWwindow* m_window;
		std::auto_ptr<RootView>  m_rootviews;
	};


}