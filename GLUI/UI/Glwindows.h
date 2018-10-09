#pragma once
#include "glfwindows.h"
#include "RootView.h"
namespace GLUI
{



	class GLwindow :public windows {
	public:
		//virtual void event(GLUI::event*; event) override;//��Ϣ����
		virtual void OnMove(int x, int y) override;
		virtual void OnSize(int w, int h) override;
		virtual bool OnClose() override;//����false��ֹ�ر�
		virtual void OnPaint() override;//ˢ��ͼ��
		virtual void OnFocus() override;
		virtual void OnLostFocus() override;
		virtual void OnMinimized() override;
		virtual void OnRestored() override;
		virtual void OnMouseMove(int x, int y) override;
		virtual void OnMouse(event::mouse_event event, event::mouse_statue statue) override;
		virtual void OnEnter() override;//�����봰��
		virtual void OnLeave() override;//����뿪����
		virtual void OnScroll(int y) override;//�������¼���+1���Ϲ���-1���¹�
		virtual void OnKey(int keycode, event::key_statue statue) override;
		virtual void OnInput(const char* value)override;//����ĵ����ַ��������Ǻ���,��תUTF8;
		virtual void OnDrop(int count, const char** paths) override;//����������ļ�·������Ŀ��·������;

	    void CreateWindows(int width, int height, char* title);
		RootView* rootview() { return m_rootviews.get(); }
		GLFWwindow* windows() { return m_window; }
		virtual void Repaint(rect* rect);
	private:
		GLFWwindow* m_window;
		std::auto_ptr<RootView>  m_rootviews;
	};


}