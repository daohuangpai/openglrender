#include "shaderhelper.h"
#include "lazytool.h"

namespace GLUI
{

	bool shaderhelper::setshader(std::shared_ptr<shader>& shader)
	{
		if (!SHARE_PTR_AVAILABLE(shader)) {
			BOOST_LOG_TRIVIAL(error) << "shaderhelper::bindshader: cannot bind to unavailable shader ";
			return false;
		}

		shader->Use();
		for (auto pm = m_uniformconfigure.begin(); pm != m_uniformconfigure.end(); pm++)
		{
			shader->set_uniform(pm->first.data(), pm->second);
		}

		for (auto pm = m_subroutineconfigure.begin(); pm != m_subroutineconfigure.end(); pm++)
		{
			if (!shader->set_subroutine(pm->first, pm->second))
				return false;
		}
		 
		return true;
			
	}


}