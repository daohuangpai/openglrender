#include "object.h"
#include "../Graph/Cube.h"
#include <boost/log/trivial.hpp>

namespace GLUI {

	const char lightsourceobjectvectex[] = { MAKE_TEXT(
		#version 420 \n
		layout(location = 0) in vec3 position; \n
		uniform mat4 model_matrix; \n
		uniform mat4 matixview; \n
		uniform mat4 projection_matrix; \n

	   void main(void)\n
	   { \n
		gl_Position = projection_matrix * matixview*(model_matrix * vec4(position, 1.0)); \n
		}
	   ) };

	const char lightsourceobjectfragment[] = {
		MAKE_TEXT(
			#version 420 core\n
		   uniform vec3 lightColor; \n
	       out vec4 color; \n
	       void main()\n
	     { \n
		  color = vec4(lightColor,1.0); \n
	      }
		)
	};

	bool lightsourceobject::InitShader()
	{
		std::shared_ptr<GLUI::shader> shader;
		if (!GLUI::shadermanger::shareshadermanger()->get(std::string("lightshader"), shader)
			||!shader->is_available())
		{
			shader.reset(new GLUI::rendershader());
			shader->createshader({ { GLUI::shader::VERTEX,(char*)lightsourceobjectvectex,true },
			                     { GLUI::shader::FRAGMENT,(char*)lightsourceobjectfragment,true }
			                      });
			if (!shader->is_available())
				return false;
			GLUI::shadermanger::shareshadermanger()->set(std::string("lightshader"), shader);
		}
		m_shader = shader;
		return true;
	}


	bool lightsourceobject::InitVao()
	{
		std::shared_ptr<GLUI::buffer> buffer(new GLUI::buffer());
		GLUI::MakeCubeArrayBufferFront(glm::vec3(1.0, 1.0, 1.0), buffer);
		std::shared_ptr<GLUI::Vao> vao(new GLUI::Vao);

		if (!vao->BindBuffer( { { buffer, sizeof(GLUI::CubePointInfo),0,3,GL_FLOAT,GL_FALSE,0 },
							 { buffer, sizeof(GLUI::CubePointInfo),1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT) } }))
			return false;
		m_vao = vao;
		return true;
	}

	bool lightsourceobject::Drow(glm::mat4& matixview, glm::mat4& perspectiveview)
	{
		std::shared_ptr<GLUI::shader>& drowshader = shader();
		if (!drowshader.get()) 
			return false;
		
		drowshader->Use();
		drowshader->set_uniform_float_vec3("lightColor", m_lightcolor[0], m_lightcolor[1], m_lightcolor[2]);
		drowshader->set_uniform_mat_4_4("model_matrix", (GLfloat*)&m_modelview);
		drowshader->set_uniform_mat_4_4("matixview", (GLfloat*)&matixview);
		drowshader->set_uniform_mat_4_4("projection_matrix", (GLfloat*)&perspectiveview);

		m_vao->DrowArray(GLUI::Vao::TRIANGLE_STRIP, 0, 36);
		return true;
	}
 
}