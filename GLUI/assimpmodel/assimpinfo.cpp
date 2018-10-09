#include "assimpinfo.h"


void printassimpmesh(aiMesh* mesh)
{
	switch (mesh->mPrimitiveTypes)
	{
 
	case aiPrimitiveType_POINT:
		BOOST_LOG_TRIVIAL(info) << "ͼԪ��ʽ����(aiPrimitiveType_POINT)";
		break;
	case aiPrimitiveType_LINE:
		BOOST_LOG_TRIVIAL(info) << "ͼԪ��ʽ����(aiPrimitiveType_LINE)";
		break;
	case aiPrimitiveType_TRIANGLE:
		BOOST_LOG_TRIVIAL(info) << "ͼԪ��ʽ������(aiPrimitiveType_TRIANGLE)";
		break;
	case aiPrimitiveType_POLYGON:
		BOOST_LOG_TRIVIAL(info) << "ͼԪ��ʽ������(aiPrimitiveType_POLYGON)";
		break;
	case _aiPrimitiveType_Force32Bit:
		BOOST_LOG_TRIVIAL(info) << "ͼԪ��ʽ��(_aiPrimitiveType_Force32Bit)";
		break;
	default:
		BOOST_LOG_TRIVIAL(info) << "ͼԪ��ʽ��δ֪";
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "��������"<< mesh->mNumVertices;
	BOOST_LOG_TRIVIAL(info) << "����������" << mesh->mNumFaces;
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		BOOST_LOG_TRIVIAL(info) << i << "������������" << mesh->mFaces[i].mNumIndices;
	}
	BOOST_LOG_TRIVIAL(info) << "������ɫ�������" << mesh->GetNumColorChannels();
	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
	{
		if (mesh->mTextureCoords[i] = nullptr)
			break;

		BOOST_LOG_TRIVIAL(info)<<"��"<< i << "�������������ά�ȣ�" << mesh->mNumUVComponents[i];
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
	
	BOOST_LOG_TRIVIAL(info)<<  "�ڵ�����" << node->mName.data;
	BOOST_LOG_TRIVIAL(info)  << "��Ը�����ķ������";
	debug_Mat_info(glm::make_mat4(&node->mTransformation.a1));
	BOOST_LOG_TRIVIAL(info)  << "�ӽڵ�������"<< node->mNumChildren;
	BOOST_LOG_TRIVIAL(info) << "����������" << node->mNumMeshes << "\r\n\r\n";

	for (int i = 0; i < node->mNumChildren; i++)
	{
		printassimpmesh(node->mChildren[i], spacenum + 1);
	}
}

