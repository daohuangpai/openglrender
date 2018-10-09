#pragma once 
#include "../common/glslvalue.h"
#include "shader.h"

namespace GLUI
{
	class shaderhelper
	{
	public:
		bool setshader(std::shared_ptr<shader>& shader);

		glslconfigure& uniform() { return m_uniformconfigure; }
		std::map<shader::SHADER_TYPE, std::map<std::string, std::string>>& subroutine() { return m_subroutineconfigure; }
 
	private:
		glslconfigure m_uniformconfigure;
		std::map<shader::SHADER_TYPE,std::map<std::string, std::string>> m_subroutineconfigure;
	};
}