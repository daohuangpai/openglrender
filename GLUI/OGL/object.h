#pragma once
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <string>
#include "../OGL/buffer.h"
#include "../OGL/Texture.h"
#include "../OGL/shader.h"
#include "../OGL/util.h"
#include "../OGL/Vao.h"

namespace GLUI {

 
	class Object {
	public:
		inline void setpoint(float x, float y, float z) { m_modelview = glm::translate(m_modelview, glm::vec3(x, y, z));}
		inline void setpoint(glm::vec3 postion) { m_modelview = glm::translate(m_modelview, postion);}
		inline void rotate(float angle,glm::vec3 vec) { m_modelview = glm::rotate(m_modelview, angle, vec); }
		glm::mat4& modelview() { return m_modelview; }

	protected:
		glm::mat4 m_modelview = glm::mat4(1.0f);
    };
 
	class lightsourceobject :public Object {
	public:
		glm::vec3& lightcolor() { return m_lightcolor; }
		bool InitShader();
		std::shared_ptr<GLUI::shader>& shader() { 
			if(!m_shader.get()||!m_shader->is_available())
				InitShader();
			return m_shader;
		}
		bool InitVao();
		bool Drow(glm::mat4& matixview, glm::mat4& perspectiveview);
	private:
		glm::vec3 m_lightcolor = glm::vec3(1.0f,1.0f,1.0f);
		std::shared_ptr<GLUI::Vao> m_vao;
		std::shared_ptr<GLUI::shader> m_shader;
	};

 

}