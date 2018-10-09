#pragma once
#include "../OGL/util.h"
#include "../model/dataarray.h"
#include "..\OGL\vao.h"


namespace COMPUTER
{
	//UV×ø±ê×ªtangent¡¢biangent×ø±ê
	bool computerBNT(IN glm::vec2* uv, IN glm::vec3* vectex,OUT glm::vec3& normal,OUT glm::vec3& tangent, OUT glm::vec3& biangent);

	struct BNT_PARAM
	{
		IN dataarray<glm::vec2>* uv = nullptr;
		IN dataarray<glm::vec3>* vectex = nullptr;
		IN dataarray<unsigned int>* element = nullptr;
		OUT dataarray<glm::vec3>* normal = nullptr;
		OUT dataarray<glm::vec3>* tangent = nullptr;
		OUT dataarray<glm::vec3>* biangent = nullptr;
		IN GLUI::Vao::DROW_TYPE drowtype;
	};

	bool computerBNTVector(BNT_PARAM& param);

}