#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../OGL/util.h"

namespace MODEL
{
	inline void assimp2glm_mat(IN aiMatrix4x4& assmat,OUT glm::mat4& glmmat)
	{
		glmmat[0][0] = assmat.a1; glmmat[0][1] = assmat.a2; glmmat[0][2] = assmat.a3; glmmat[0][3] = assmat.a4;
		glmmat[1][0] = assmat.b1; glmmat[1][1] = assmat.b2; glmmat[1][2] = assmat.b3; glmmat[1][3] = assmat.b4;
		glmmat[2][0] = assmat.c1; glmmat[2][1] = assmat.c2; glmmat[2][2] = assmat.c3; glmmat[2][3] = assmat.c4;
	}
	inline void glm_mat2assimp(IN glm::mat4& glmmat, OUT aiMatrix4x4& assmat)
	{
		assmat.a1 = glmmat[0][0]; assmat.a2 = glmmat[0][1]; assmat.a3= glmmat[0][2];  assmat.a4 = glmmat[0][3];
		assmat.b1 = glmmat[1][0]; assmat.b2 = glmmat[1][1]; assmat.b3= glmmat[1][2]; assmat.b4= glmmat[1][3];
		assmat.c1= glmmat[2][0];assmat.c2= glmmat[2][1];  assmat.c3= glmmat[2][2]; assmat.c4= glmmat[2][3];
	}
}