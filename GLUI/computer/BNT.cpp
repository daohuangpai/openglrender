#include "BNT.h"

namespace COMPUTER
{
	//http://wiki.jikexueyuan.com/project/modern-opengl-tutorial/tutorial26.html
	bool computerBNT(IN glm::vec2* uv, IN glm::vec3* vectex, OUT glm::vec3& normal, OUT glm::vec3& tangent, OUT glm::vec3& biangent)
	{
		glm::mat2x3 VEC;
		VEC[0] = vectex[1] - vectex[0];
		VEC[1] = vectex[2] - vectex[0];
		glm::mat2x2 UV;
		UV[0] = uv[1] - uv[0];
		UV[1] = uv[2] - uv[0];
		if (UV[0].x / UV[1].x == UV[0].y / UV[1].y) {
			BOOST_LOG_TRIVIAL(error) << "computerBNT: 纹理坐标线性相关无法求逆:("<< UV[0].x<<","<< UV[0].y<<")," << UV[1].x << ","<<UV[1].y<<")";
			return false;
		}
		UV = glm::inverse(UV);
		glm::mat2x3 TB = VEC*UV;
		normal = glm::cross(TB[0], TB[1]);
		tangent = TB[0];
		biangent = TB[1];
		return true;
	}

	//每个顶点的 tangent/bitangent 值由共享这个顶点的所有三角面的平均 tangent/bitangent 值确定（这与顶点法线是一样的）。这样做的原因是使整个三角面的效果比较平滑，防止相邻三角面之间的不平滑过渡。
	bool computerBNTVector(BNT_PARAM& param)
	{

		CHECKFALSE((param.vectex != nullptr) && (param.uv != nullptr)&&(param.normal!=nullptr)&&(param.tangent!=nullptr)&&(param.biangent!=nullptr))
		int size = (param.element != nullptr) ? param.element->get_size() : param.vectex->get_size();

		dataarray<unsigned int> normalnums;
		dataarray<unsigned int> tangentnums;
		dataarray<unsigned int> biangentnums;
		normalnums.init(size);
		memset(normalnums.m_data, 0, normalnums.get_size()*sizeof(unsigned int));
		tangentnums.init(size); 
		memset(tangentnums.m_data, 0, tangentnums.get_size() * sizeof(unsigned int));
	    biangentnums.init(size); 
		memset(biangentnums.m_data, 0, biangentnums.get_size() * sizeof(unsigned int));

		param.normal->init(size);
		memset(param.normal->m_data, 0, param.normal->get_size() * sizeof(glm::vec3));
		param.tangent->init(size);
		memset(param.tangent->m_data, 0, param.tangent->get_size() * sizeof(glm::vec3));
		param.biangent->init(size);
		memset(param.biangent->m_data, 0, param.biangent->get_size() * sizeof(glm::vec3));
		switch (param.drowtype)
		{
		case GLUI::Vao::TRIANGLES:
		   {
			 CHECKFALSE((size % 3) == 0)
			 int length  = size / 3;
			 glm::vec3 vectex[3];
			 glm::vec2 uv[3];
			 glm::vec3 normal;
			 glm::vec3 tangent;
			 glm::vec3 biangent;
			 for (int i = 0; i < length; i++)
			 {
				 unsigned int pos0 = (param.element != nullptr) ? (*param.element)[i*3] : i * 3;
				 unsigned int pos1 = (param.element != nullptr) ? (*param.element)[i*3+1] : i * 3 + 1;
				 unsigned int pos2 = (param.element != nullptr) ? (*param.element)[i*3+2] : i * 3 + 2;
				 vectex[0] = (*param.vectex)[pos0];
				 vectex[1] = (*param.vectex)[pos1];
				 vectex[2] = (*param.vectex)[pos2];
				 uv[0] = (*param.uv)[pos0];
				 uv[1] = (*param.uv)[pos1];
				 uv[2] = (*param.uv)[pos2];
				 if (!computerBNT(uv, vectex, normal, tangent, biangent))
					 return false;
				 normal = glm::normalize(normal);
				 tangent = glm::normalize(tangent);
				 biangent = glm::normalize(biangent);
				 normalnums[pos0] += 1;
				 normalnums[pos1] += 1;
				 normalnums[pos2] += 1;
				 tangentnums[pos0] += 1;
				 tangentnums[pos1] += 1;
				 tangentnums[pos2] += 1;
				 biangentnums[pos0] += 1;
				 biangentnums[pos1] += 1;
				 biangentnums[pos2] += 1;
				 (*param.normal)[pos0] += normal;
				 (*param.normal)[pos1] += normal;
				 (*param.normal)[pos2] += normal;
				 (*param.tangent)[pos0] += tangent;
				 (*param.tangent)[pos1] += tangent;
				 (*param.tangent)[pos2] += tangent;
				 (*param.biangent)[pos0] += biangent;
				 (*param.biangent)[pos1] += biangent;
				 (*param.biangent)[pos2] += biangent;
			 }
			 for (int i = 0; i < size; i++)
			 {
				 (*param.normal)[i] /= normalnums[i];
				 (*param.tangent)[i] /= tangentnums[i];
				 (*param.biangent)[i] /= biangentnums[i];
			 }
		   }
			break;
		default:
			BOOST_LOG_TRIVIAL(error) << "computerBNTVector: only support GLUI::Vao::TRIANGLES";
			return false;
			break;
		 }
		return true;
	}

}