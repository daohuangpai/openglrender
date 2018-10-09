#pragma once
#include "util.h"

namespace GLUI {

	class camera {
	public:
		void move_front_back(float z);//z>0前，小于后
		void move_up_down(float y);//y>0上边，小于下边
		void move_left_right(float x);//x>0左边，小于0右边
		void move(float x,float y,float z);
		void rotatePosition(float angle);//旋转位置，顾名思义，是旋转当前位置但是还是看向同一个点;
		void rotateEye(float angle);//旋转眼睛，顾名思义，是旋转眼睛看向的点的位置
		void computematixview() {  m_matixview = glm::lookAt(m_position, m_lookat_position, m_up); }//见<glm关于相机视角坐标系转化原理.nb>
		void computeperspectiveview() { m_perspectiveview = glm::perspective(fovy, aspect, zNear, zFar); }//见<opengl投影矩阵.nb>
		//如果看的方向和头的方向一致glm::lookAt根据<glm关于相机视角坐标系转化原理.nb>缺少另一个基，所以无法构成
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

		//转眼睛坐标，第一人称坐标
		glm::vec3 m_up = glm::vec3(0, 1, 0);
		glm::vec3 m_lookat_position = glm::vec3(0, 0, 0);
		glm::vec3 m_position = glm::vec3(0, 0, -1);
		glm::mat4 m_matixview;

		//转投影坐标
		float fovy = PI/2.0;
		float aspect = 1;
		float zNear = 10;
		float zFar = 100;
		glm::mat4 m_perspectiveview;

	};

















}