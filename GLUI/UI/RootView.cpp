#include "RootView.h"
#include <boost/log/trivial.hpp>


namespace GLUI {


	//���������ԣ���ĩβ�Ľڵ������ߵ����ȼ�
	view* RootView::HitView(view* root, int x, int y)
	{
		if (root->HitTest(x, y))
		{
			//�����������
			int nx = x - root->m_rect.x;
			int ny = y - root->m_rect.y;
			for (std::vector<view*>::iterator itr = root->m_son_view.begin();
				itr != root->m_son_view.end(); itr++) {
				view* v = HitView((*itr), nx, ny);
				if (v)  return v;
			}

			root->Enter();
			root->OnMouseMove(nx, ny);
			return root;

		}

		return nullptr;
	}


	view* RootView::GetHitView(view* root, int x, int y)
	{
		if (root->HitTest(x, y))
		{
			//�����������
			int nx = x - root->m_rect.x;
			int ny = y - root->m_rect.y;
			for (std::vector<view*>::iterator itr = root->m_son_view.begin();
				itr != root->m_son_view.end(); itr++) {
				view* v = GetHitView((*itr), nx, ny);
				if (v)  return v;
			}
			return root;

		}

		return nullptr;
	}

	view* RootView::GetEnterView(view* root)
	{
		if (root->is_statue(IS_ENTER)) return root;

		for (std::vector<view*>::iterator itr = root->m_son_view.begin();
			itr != root->m_son_view.end(); itr++) {
			view* v = GetEnterView((*itr));
			if (v)  return v;
		}

		return nullptr;

	}

	//ģʽ��Ϣ��view���ȼ���
	//��views��������ô���
	void RootView::MouseMove(int x, int y)
	{
		int ex = x, ey = y;
		view* hitview = GetHitView(this,ex,ey);
		if (m_enterview&&(m_enterview!= hitview))
			m_enterview->Leave();	
		m_enterview = hitview;
		if (hitview)
			hitview->Enter();
	}


	void RootView::Mouse(event::mouse_event event, event::mouse_statue statue)
	{
		int staute = event|statue;
        //û�п��õ�view�򷵻�
		if (!m_enterview) { return; }
		switch (staute)
		{
		case event::LB_CLICK:
			m_enterview->set_statue(IS_LB_DOWN);
			//�������һ������Ŀؼ��Ҳ��ǵ�ǰ���µ���ʹ��ʧȥ����
			//���ü���ĺ�enter��viewһ��
			if (m_activeview&&m_activeview!= m_enterview)
				m_activeview->LostFocus();
			m_activeview = m_enterview;
			m_activeview->Focus();
			break;
		case event::LB_RELEASE:
			//ֻ��ͬʱ��һ����Ч��view�а��º��ͷŰ�������������Ч
			if (m_enterview && m_enterview == m_activeview) {
				m_enterview->OnLBclick();
			}
			break;
		case event::RB_CLICK:
			m_enterview->set_statue(IS_RB_DOWN);
			//�������һ������Ŀؼ��Ҳ��ǵ�ǰ���µ���ʹ��ʧȥ����
			//���ü���ĺ�enter��viewһ��
			if (m_activeview&&m_activeview != m_enterview)
				m_activeview->LostFocus();
			m_activeview = m_enterview;
			m_enterview->Focus();
			break;
		case event::RB_RELEASE:
			//ֻ��ͬʱ��һ����Ч��view�а��º��ͷŰ�������������Ч
			if (m_enterview && m_enterview == m_activeview) {
				m_enterview->OnRBclick();
			}
			break;
		case event::MB_CLICK:
			m_enterview->set_statue(IS_MB_DOWN);
			//�������һ������Ŀؼ��Ҳ��ǵ�ǰ���µ���ʹ��ʧȥ����
			//���ü���ĺ�enter��viewһ��
			if (m_activeview&&m_activeview != m_enterview)
				m_activeview->LostFocus();
			m_activeview = m_enterview;
			m_enterview->Focus();
			break;
		case event::MB_RELEASE:
			//ֻ��ͬʱ��һ����Ч��view�а��º��ͷŰ�������������Ч
			if (m_enterview && m_enterview == m_activeview) {
				m_enterview->OnMBclick();
			}
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "unkonw mouse event";
			break;
		}

	}




	void RootView::Key(int keycode, event::key_statue statue)
	{
		if (m_activeview) {
			m_activeview->OnKey(keycode, statue);
		}
	}

	void RootView::Input(const char* value)
	{
		if (m_activeview) {
			m_activeview->OnInput(value);
		}
	}

	void RootView::Drop(int count, const char** paths)
	{
		if (m_activeview) {
			m_activeview->OnDrop(count, paths);
		}
	}

	void RootView::Scroll(int y)
	{
		if (m_activeview) {
			m_activeview->OnScroll(y);
		}
	}

}