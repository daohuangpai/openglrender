#include "Glwindows.h"
#include <boost/log/trivial.hpp>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace GLUI
{
	void GLwindow::CreateWindows(int width, int height, char* title)
	{
		m_window= glfwCreateWindow(width, height, title, NULL, NULL);
		if (!m_window) {
			BOOST_LOG_TRIVIAL(error) << "faled to create windows";
			return;
		}
		m_rootviews.reset(new RootView);
		m_rootviews->m_window = this;
		m_rootviews->set_rect(0, 0, width, height);
		ConnectGlf2Window(this, m_window);
		//先设为当前句柄，否则opengl获取图片纹理失败
		glfwMakeContextCurrent(m_window);
	}

	void GLwindow::OnPaint()
	{
		glfwMakeContextCurrent(m_window);
		//paint
		//BOOST_LOG_TRIVIAL(info) << "paint.......";
		m_rootviews->Drow(m_rootviews->get_size());
		glfwSwapBuffers(m_window);
	}


	void GLwindow::OnMove(int x, int y)
	{
		
	}

	void GLwindow::OnSize(int w, int h)
	{
		m_rootviews->set_rect(0, 0, w, h);
	}

	bool GLwindow::OnClose()
	{
		return true;
	}

	void GLwindow::OnFocus()
	{

	}

	void GLwindow::OnLostFocus()
	{
		m_rootviews->WindowsLostFouce();
	}

	void GLwindow::OnMinimized()
	{

	}


	void GLwindow::OnRestored()
	{

	}


	void GLwindow::OnMouseMove(int x, int y)
	{
		m_rootviews->MouseMove(x, y);
	}



	void GLwindow::OnMouse(event::mouse_event event, event::mouse_statue statue)
	{
		m_rootviews->Mouse(event, statue);
	}


	void GLwindow::OnEnter()
	{

	}

	void GLwindow::OnLeave()
	{

	}


	void GLwindow::OnScroll(int y)
	{
		m_rootviews->OnScroll(y);
	}


	void GLwindow::OnKey(int keycode, event::key_statue statue)
	{
		m_rootviews->Key(keycode, statue);
	}

	void GLwindow::OnInput(const char* value)
	{
		m_rootviews->Input(value);
	}

	void GLwindow::OnDrop(int count, const char** paths)
	{
		m_rootviews->Drop(count, paths);
	}

	void GLwindow::Repaint(rect* rect)
	{
		::PostMessage(glfwGetWin32Window(m_window),WM_PAINT,0,0);
	}

}