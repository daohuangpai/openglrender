#include "assimpinfo.h"


void printassimpmesh(aiMesh* mesh)
{
	switch (mesh->mPrimitiveTypes)
	{
 
	case aiPrimitiveType_POINT:
		BOOST_LOG_TRIVIAL(info) << "图元格式：点(aiPrimitiveType_POINT)";
		break;
	case aiPrimitiveType_LINE:
		BOOST_LOG_TRIVIAL(info) << "图元格式：线(aiPrimitiveType_LINE)";
		break;
	case aiPrimitiveType_TRIANGLE:
		BOOST_LOG_TRIVIAL(info) << "图元格式：三角(aiPrimitiveType_TRIANGLE)";
		break;
	case aiPrimitiveType_POLYGON:
		BOOST_LOG_TRIVIAL(info) << "图元格式：曲线(aiPrimitiveType_POLYGON)";
		break;
	case _aiPrimitiveType_Force32Bit:
		BOOST_LOG_TRIVIAL(info) << "图元格式：(_aiPrimitiveType_Force32Bit)";
		break;
	default:
		BOOST_LOG_TRIVIAL(info) << "图元格式：未知";
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "顶点数："<< mesh->mNumVertices;
	BOOST_LOG_TRIVIAL(info) << "索引表数：" << mesh->mNumFaces;
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		BOOST_LOG_TRIVIAL(info) << i << "索引顶点数：" << mesh->mFaces[i].mNumIndices;
	}
	BOOST_LOG_TRIVIAL(info) << "顶点颜色表个数：" << mesh->GetNumColorChannels();
	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
	{
		if (mesh->mTextureCoords[i] = nullptr)
			break;

		BOOST_LOG_TRIVIAL(info)<<"第"<< i << "纹理表：纹理坐标维度：" << mesh->mNumUVComponents[i];
	}
}

std::string space(int spacenum)
{
	std::string ret = " ";
	ret.resize(spacenum + 1);
	memset(&ret[0], ' ', spacenum);
	ret[spacenum] = 0;
	return ret;
}

void printassimpmesh(aiNode* node,int spacenum)
{
	
	BOOST_LOG_TRIVIAL(info)<<  "节点名：" << node->mName.data;
	BOOST_LOG_TRIVIAL(info)  << "相对父矩阵的仿射矩阵：";
	debug_Mat_info(glm::make_mat4(&node->mTransformation.a1));
	BOOST_LOG_TRIVIAL(info)  << "子节点数量："<< node->mNumChildren;
	BOOST_LOG_TRIVIAL(info) << "网格数量：" << node->mNumMeshes << "\r\n\r\n";

	for (int i = 0; i < node->mNumChildren; i++)
	{
		printassimpmesh(node->mChildren[i], spacenum + 1);
	}
}

