#pragma once
#include "event.h"
#include "util.h"

namespace GLUI
{


	class windows {
	public:
		//virtual void event(event* event) = 0;//消息处理
		virtual void OnMove(int x,int y) = 0;
		virtual void OnSize(int w, int h) = 0;
		virtual bool OnClose() = 0;//返回false禁止关闭
		virtual void OnPaint() = 0;//刷新图像
		virtual void OnFocus() = 0;
		virtual void OnLostFocus() = 0;
		virtual void OnMinimized() = 0;
		virtual void OnRestored() = 0;
		virtual void OnMouseMove(int x, int y) = 0;
		virtual void OnMouse(event::mouse_event event, event::mouse_statue statue) = 0;
		virtual void OnEnter() = 0;//鼠标进入窗口
		virtual void OnLeave() = 0;//鼠标离开窗口
		virtual void OnScroll(int y) = 0;//鼠标滚动事件，+1向上滚，-1向下滚
		virtual void OnKey(int keycode, event::key_statue statue) = 0;
		virtual void OnInput(const char* value) = 0;//输入的单个字符，可以是汉字,已转UTF8;
		virtual void OnDrop(int count,const char** paths) = 0;//输入拖入的文件路径，数目，路径数组;

		virtual void Repaint(rect* rect) = 0;
    };


























}