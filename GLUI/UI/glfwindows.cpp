#include "glfwindows.h"
#include <boost/log/trivial.hpp>
 

namespace GLUI
{


	void window_pos_callback(GLFWwindow* window, int x, int y)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		w->OnMove(x, y);
	}


	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		w->OnSize(width, height);
	}

	static void window_close_callback(GLFWwindow* window)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		int v = w->OnClose()? GLFW_TRUE: GLFW_FALSE;
		glfwSetWindowShouldClose(window, v);
	}


	void window_refresh_callback(GLFWwindow* window)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		w->OnPaint();
	}

	static void window_focus_callback(GLFWwindow* window, int focused)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		if (focused) {
			w->OnFocus();
		}
		else {
			w->OnLostFocus();
		}
	}

	void window_iconify_callback(GLFWwindow* window, int iconified)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		if (iconified) {
			w->OnMinimized();
		}
		else {
			w->OnRestored();
		}
	}

	
	void cursor_position_callback(GLFWwindow* window, double x, double y)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		//int sizex, sizey;
		//glfwGetWindowSize(window, &sizex, &sizey);
		w->OnMouseMove(x, y);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{	
		int evl;
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			evl = GLUI::event::LEFT_BUTTON;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			evl = GLUI::event::RIGHT_BUTTON;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			evl = GLUI::event::MIDDLE_BUTTON;
			break;
		default:
			BOOST_LOG_TRIVIAL(error) << "unkonw mouse button";
			return;
			break;
		}
		int statue;
		switch (action)
		{
		case GLFW_PRESS:
			statue = GLUI::event::CLICK;
			break;
		case GLFW_RELEASE:
			statue = GLUI::event::RELEASE;
			break;
		//case GLFW_REPEAT:
		//	statue = GLUI::event::DOUBLECLICK;
		//	break;
		default:
			BOOST_LOG_TRIVIAL(error) << "unkonw mouse statue";
			return;
			break;
		}
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		w->OnMouse(evl, statue);
	}

    void cursor_enter_callback(GLFWwindow* window, int entered)
	{
		windows* w = (windows*)glfwGetWindowUserPointer(window);
		if (entered) {
			w->OnEnter();
		}
		else {
			w->OnLeave();
		}
	}


   void scroll_callback(GLFWwindow* window, double x, double y)
	{
	    windows* w = (windows*)glfwGetWindowUserPointer(window);
		w->OnScroll(y);
	}


   void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
   {
	   int statue;
	   switch (action)
	   {
	   case GLFW_PRESS:
		   statue = GLUI::event::CLICK;
		   break;
	   case GLFW_RELEASE:
		   statue = GLUI::event::RELEASE;
		   break;
	   //case GLFW_REPEAT:
		//   statue = GLUI::event::DOUBLECLICK;
		//   break;
	   default:
		   BOOST_LOG_TRIVIAL(error) << "unkonw mouse statue";
		   return;
		   break;
	   }
	   windows* w = (windows*)glfwGetWindowUserPointer(window);
	   w->OnKey(key, statue);
   }


   const char* get_character_string(int codepoint)
   {
	   // This assumes UTF-8, which is stupid
	   static char result[6 + 1];
	 
	   //int length = wctomb(result, codepoint);
	   int length = 0;
       #if defined(WIN32) && !defined(CYGWIN) && !defined(__MINGW32__) && !defined(__MINGW64__)
	   wctomb_s(&length, result, sizeof(result), (wchar_t)(codepoint & 0x7FFFFFFF));
       #else
	   length = wctomb(result, codepoint);
        #endif
	   if (length == -1)
		   length = 0;

	   result[length] = '\0';
	   return result;
   }
   static void char_callback(GLFWwindow* window, unsigned int codepoint)
   {
	   windows* w = (windows*)glfwGetWindowUserPointer(window);
	   w->OnInput(get_character_string(codepoint));
   }

   static void drop_callback(GLFWwindow* window, int count, const char** paths)
   {
	   windows* w = (windows*)glfwGetWindowUserPointer(window);
	   w->OnDrop(count, paths);
   }

	void ConnectGlf2Window(windows* windows, GLFWwindow* window)
	{
		glfwSetWindowUserPointer(window, windows);

		glfwSetWindowPosCallback(window, window_pos_callback);
		glfwSetWindowSizeCallback(window, window_size_callback);
		glfwSetWindowCloseCallback(window, window_close_callback);
		glfwSetWindowRefreshCallback(window, window_refresh_callback);
		glfwSetWindowFocusCallback(window, window_focus_callback);
		glfwSetWindowIconifyCallback(window, window_iconify_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetCursorEnterCallback(window, cursor_enter_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetKeyCallback(window, key_callback);
		glfwSetCharCallback(window, char_callback);
		glfwSetDropCallback(window, drop_callback);
	}

	int getKeyMods(void)
	{
		int mods = 0;

		if (GetKeyState(VK_SHIFT) & (1 << 31))
			mods |= GLFW_MOD_SHIFT;
		if (GetKeyState(VK_CONTROL) & (1 << 31))
			mods |= GLFW_MOD_CONTROL;
		if (GetKeyState(VK_MENU) & (1 << 31))
			mods |= GLFW_MOD_ALT;
		if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1 << 31))
			mods |= GLFW_MOD_SUPER;

		return mods;
	}
}