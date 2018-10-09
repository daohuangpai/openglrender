#pragma once
#include "event.h"
#include "util.h"

namespace GLUI
{


	class windows {
	public:
		//virtual void event(event* event) = 0;//��Ϣ����
		virtual void OnMove(int x,int y) = 0;
		virtual void OnSize(int w, int h) = 0;
		virtual bool OnClose() = 0;//����false��ֹ�ر�
		virtual void OnPaint() = 0;//ˢ��ͼ��
		virtual void OnFocus() = 0;
		virtual void OnLostFocus() = 0;
		virtual void OnMinimized() = 0;
		virtual void OnRestored() = 0;
		virtual void OnMouseMove(int x, int y) = 0;
		virtual void OnMouse(event::mouse_event event, event::mouse_statue statue) = 0;
		virtual void OnEnter() = 0;//�����봰��
		virtual void OnLeave() = 0;//����뿪����
		virtual void OnScroll(int y) = 0;//�������¼���+1���Ϲ���-1���¹�
		virtual void OnKey(int keycode, event::key_statue statue) = 0;
		virtual void OnInput(const char* value) = 0;//����ĵ����ַ��������Ǻ���,��תUTF8;
		virtual void OnDrop(int count,const char** paths) = 0;//����������ļ�·������Ŀ��·������;

		virtual void Repaint(rect* rect) = 0;
    };


























}