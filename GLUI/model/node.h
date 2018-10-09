#pragma once

#include "mesh.h"
#include "../OGL/treestruct.h"
#include "../OGL/renderbuffer.h"
#include "../common/configure.h"
#include "../common/glslvalue.h"
#include "../OGL/lazytool.h"
#include "material.h"


namespace MODEL
{
	class model;
	class node:public simpletree<node> {
	public:
		struct param
		{
			unsigned int meshindex;
			unsigned int materialindex;
		};
		AABB get_AABB(model* mod);
		std::vector<param>& get_param() { return m_param; };
		SET_GET_FUN(glm::mat4,transformation)
		SET_GET_FUN(std::string, name)
	private:
		glm::mat4 m_transformation;
		std::vector<param> m_param;
		std::string m_name;
	};




}