#pragma once
#include "util.h"

namespace GLUI {

	class camera {
	public:
		void move_front_back(float z);//z>0ǰ��С�ں�
		void move_up_down(float y);//y>0�ϱߣ�С���±�
		void move_left_right(float x);//x>0��ߣ�С��0�ұ�
		void move(float x,float y,float z);
		void rotatePosition(float angle);//��תλ�ã�����˼�壬����ת��ǰλ�õ��ǻ��ǿ���ͬһ����;
		void rotateEye(float angle);//��ת�۾�������˼�壬����ת�۾�����ĵ��λ��
		void computematixview() {  m_matixview = glm::lookAt(m_position, m_lookat_position, m_up); }//��<glm��������ӽ�����ϵת��ԭ��.nb>
		void computeperspectiveview() { m_perspectiveview = glm::perspective(fovy, aspect, zNear, zFar); }//��<openglͶӰ����.nb>
		//������ķ����ͷ�ķ���һ��glm::lookAt����<glm��������ӽ�����ϵת��ԭ��.nb>ȱ����һ�����������޷�����
		bool is_available() 
		{ 
			glm::vec3 v = m_lookat_position - m_position;
			float per = v[0] / m_up[0];
			return (per == v[1] / m_up[1]) && (per = v[2] / m_up[2]);
		}

		void log()
		{
			debug_vec_info(m_up)
			debug_vec_info(m_lookat_position)
			debug_vec_info(m_position)
		}

		//ת�۾����꣬��һ�˳�����
		glm::vec3 m_up = glm::vec3(0, 1, 0);
		glm::vec3 m_lookat_position = glm::vec3(0, 0, 0);
		glm::vec3 m_position = glm::vec3(0, 0, -1);
		glm::mat4 m_matixview;

		//תͶӰ����
		float fovy = PI/2.0;
		float aspect = 1;
		float zNear = 10;
		float zFar = 100;
		glm::mat4 m_perspectiveview;

	};

















}