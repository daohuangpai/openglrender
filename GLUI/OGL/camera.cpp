#include "camera.h"
#include <boost/log/trivial.hpp>

namespace GLUI {

	void camera::rotateEye(float angle)
	{
		//得到旧的看向的向量
		glm::vec3 v = m_lookat_position - m_position;
		//计算旋转之后的新的向量
		glm::vec3 rv = glm::rotatev(v,angle, m_up);
		//将新向量加上当前位置，得到新的视点
		m_lookat_position = rv + m_position;

	}


	void camera::rotatePosition(float angle)
	{
		glm::vec3 v = m_position - m_lookat_position  ;
		glm::vec3 rv = glm::rotatev(v, angle, m_up);
		m_position = rv + m_lookat_position;
	}

	void camera::move(float x, float y, float z)
	{
		m_position = m_position + glm::vec3(x, y, z);
	}

	void camera::move_left_right(float x)
	{
		glm::vec3 lv = glm::normalize(glm::cross(m_up, m_lookat_position - m_position));
		m_position = m_position + lv*x;
		m_lookat_position = m_lookat_position + lv*x;
	}


	void camera::move_up_down(float y)
	{
		glm::vec3 tv = glm::normalize(m_up);
		m_position = m_position + tv*y;
		m_lookat_position = m_lookat_position + tv*y;
	}

	void camera::move_front_back(float z)
	{
		glm::vec3 zv = glm::normalize(m_lookat_position - m_position);
		m_position = m_position + zv*z;
		m_lookat_position = m_lookat_position + zv*z;
	}


}