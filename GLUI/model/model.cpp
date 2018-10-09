#include "model.h"
#include "../common/commom.h"
#include "animation.h"
#include "common.h"
namespace MODEL
{

	bool model::ergodic_node(MODEL::node* node, const std::function<bool(MODEL::node*)> &AccessFuntion)
	{
		if (AccessFuntion(node))
			return true;

		std::vector<std::unique_ptr<MODEL::node>>& childs = node->childs();
		for (auto itr = childs.begin(); itr != childs.end(); itr++)
		{
			if (ergodic_node(itr->get(), AccessFuntion))
				return true;
		}

		return false;
	}


	void model::pasernode(const aiScene* scene, node* node, aiNode* ainode)
	{
		node->set_name(ainode->mName.C_Str());
		node->set_transformation(glm::make_mat4(&ainode->mTransformation.a1));
		std::vector<node::param>& params = node->get_param();
		params.resize(ainode->mNumMeshes);
		for (auto i = 0; i < ainode->mNumMeshes; i++)
		{
			params[i].meshindex = ainode->mMeshes[i];		
			params[i].materialindex = scene->mMeshes[ainode->mMeshes[i]]->mMaterialIndex;
		}
		for (int i = 0; i < ainode->mNumChildren; i++)
		{
			MODEL::node* nd = new MODEL::node;
			node->addchild(nd);
			pasernode(scene, nd, ainode->mChildren[i]);
		}
	}

	bool model::get_node_transform(IN int animaindex,IN double sec,IN node* node,OUT glm::mat4& mat)
	{
		const aiScene *scene = m_importer.GetScene();
		assert(scene && (scene->mFlags != AI_SCENE_FLAGS_INCOMPLETE)
			&& scene->mRootNode && (scene->mNumAnimations > animaindex));
		//if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		//{
		//	BOOST_LOG_TRIVIAL(error) << "no scene";
		//	return false;
		//}

		//if (scene->mNumAnimations <= animaindex)
		//{
		//	BOOST_LOG_TRIVIAL(error) << "out of Animations size";
		//	return false;
		//}
		//关键帧
		const aiAnimation* Animation = scene->mAnimations[animaindex];
		const aiNodeAnim* NodeAnim = nullptr;
		for (int i = 0; i < Animation->mNumChannels; i++) {
		    //通道(一个关节对应的关键帧的集合)
			const aiNodeAnim* pNodeAnim = Animation->mChannels[i];
			if (strcmp(pNodeAnim->mNodeName.data, node->get_name().data()) == 0) {
				NodeAnim = pNodeAnim;
				break;
			}
		}
		//动画用不到直接用原始的
		if (NodeAnim == nullptr) {
			mat = node->get_transformation();
			return true;
		}

		//计算时间
		//一秒的tick数
		double TicksPerSecond = (Animation->mTicksPerSecond == 0) ? 25.0f : Animation->mTicksPerSecond;
		//计算当前tick数
		double pertick = TicksPerSecond*sec;
		//得到实际上要到的tick数
		double AnimationTime = fmod(pertick, Animation->mDuration);
		//1、计算缩放
		aiVector3D Scaling(1.0f,1.0f,1.0f);
		//只有一个固定一个
		if (NodeAnim->mNumScalingKeys == 1) {
			Scaling = NodeAnim->mScalingKeys[0].mValue;
		}
		else {
		//多个则取合适位置的
			uint start_index = 0,next_index = 0;
			for (uint i = 0; i < NodeAnim->mNumScalingKeys - 1; i++) {
				if (AnimationTime <NodeAnim->mScalingKeys[i + 1].mTime) {
					start_index = i;
					next_index = i + 1;
					break;
				}
			}
			//得到间隔时间
			float DeltaTime = (float)(NodeAnim->mScalingKeys[next_index].mTime - NodeAnim->mScalingKeys[start_index].mTime);
			//得到当前时间在其中的比例时间
			float Factor = (AnimationTime - (float)NodeAnim->mScalingKeys[start_index].mTime) / DeltaTime;
			const aiVector3D& Start = NodeAnim->mScalingKeys[start_index].mValue;
			const aiVector3D& End = NodeAnim->mScalingKeys[next_index].mValue;
			aiVector3D Delta = End - Start;
			Scaling = Start + Factor*Delta;
		}

		//2、计算旋转
		aiQuaternion RotationQ;
		if (NodeAnim->mNumRotationKeys == 1) {
			RotationQ = NodeAnim->mRotationKeys[0].mValue;
		}
		else {
			//多个则取合适位置的
			uint start_index = 0, next_index = 0;
			for (uint i = 0; i < NodeAnim->mNumRotationKeys - 1; i++) {
				if (AnimationTime <NodeAnim->mRotationKeys[i + 1].mTime) {
					start_index = i;
					next_index = i+1;
					break;
				}
			}
			//得到间隔时间
			float DeltaTime = (float)(NodeAnim->mRotationKeys[next_index].mTime - NodeAnim->mRotationKeys[start_index].mTime);
			//得到当前时间在其中的比例时间
			float Factor = (AnimationTime - (float)NodeAnim->mRotationKeys[start_index].mTime) / DeltaTime;
			const aiQuaternion& Start = NodeAnim->mRotationKeys[start_index].mValue;
			const aiQuaternion& End = NodeAnim->mRotationKeys[next_index].mValue;
			aiQuaternion::Interpolate(RotationQ, Start, End, Factor);
			RotationQ = RotationQ.Normalize();
		}

		//3、计算平移
		aiVector3D position(0.0f, 0.0f, 0.0f);;
		if (NodeAnim->mNumPositionKeys == 1) {
			position = NodeAnim->mPositionKeys[0].mValue;
		}
		else {
			//多个则取合适位置的
			uint start_index = 0, next_index = 0;
			for (uint i = 0; i < NodeAnim->mNumPositionKeys - 1; i++) {
				if (AnimationTime <NodeAnim->mPositionKeys[i + 1].mTime) {
					start_index = i;
					next_index = i + 1;
					break;
				}
			}
			//得到间隔时间
			float DeltaTime = (float)(NodeAnim->mPositionKeys[next_index].mTime - NodeAnim->mPositionKeys[start_index].mTime);
			//得到当前时间在其中的比例时间
			float Factor = (AnimationTime - (float)NodeAnim->mPositionKeys[start_index].mTime) / DeltaTime;
			const aiVector3D& Start = NodeAnim->mPositionKeys[start_index].mValue;
			const aiVector3D& End = NodeAnim->mPositionKeys[next_index].mValue;
			aiVector3D Delta = End - Start;
			position = Start + Factor*Delta;
		}

		//计算矩阵
		glm::mat4 scallmat = glm::scale_slow(glm::mat4(1.0f), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		aiMatrix3x3t<ai_real> assimprmat = RotationQ.GetMatrix();
		glm::mat4 rotatemat;
		rotatemat[0][0] = assimprmat.a1; rotatemat[0][1] = assimprmat.a2; rotatemat[0][2] = assimprmat.a3; rotatemat[0][3] = 0.0f;
		rotatemat[1][0] = assimprmat.b1; rotatemat[1][1] = assimprmat.b2; rotatemat[1][2] = assimprmat.b3; rotatemat[1][3] = 0.0f;
		rotatemat[2][0] = assimprmat.c1; rotatemat[2][1] = assimprmat.c2; rotatemat[2][2] = assimprmat.c3; rotatemat[2][3] = 0.0f;
		rotatemat[3][0] = 0.0f; rotatemat[3][1] = 0.0f; rotatemat[3][2] = 0.0f; rotatemat[3][3] = 1.0f;
		glm::mat4 tranmat = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));

		mat = tranmat*rotatemat*scallmat;
		return true;
	}

	bool model::load(std::string path, int loadtype)
	{
		const aiScene *scene = m_importer.ReadFile(path, loadtype);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			BOOST_LOG_TRIVIAL(error) << "model::load: Assimp failed to load model at path: " << path;
			return nullptr;
		}
		//mesh
		m_meshs.resize(scene->mNumMeshes);
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			m_meshs[i].reset(new mesh);
			mesh* mh = dynamic_cast<mesh*>(m_meshs[i].get());
			aiMesh* aimh = scene->mMeshes[i];
			
			switch (aimh->mPrimitiveTypes)
			{
			case aiPrimitiveType_POINT:
				mh->set_drowtype(Vao::POINTS);
				break;
			case aiPrimitiveType_LINE:
				mh->set_drowtype(Vao::LINES);
				break;
			case aiPrimitiveType_TRIANGLE:
				mh->set_drowtype(Vao::TRIANGLES);
				break;
			case aiPrimitiveType_POLYGON:
				BOOST_LOG_TRIVIAL(error) << "model::load:  cannot support aiPrimitiveType_POLYGON now";
				return false;
				break;
			case _aiPrimitiveType_Force32Bit:
				BOOST_LOG_TRIVIAL(error) << "model::load: cannot support _aiPrimitiveType_Force32Bit now";
				return false;
				break;
			default:
				BOOST_LOG_TRIVIAL(error) << "model::load:  unknow type now";
				return false;
				break;
			}

			std::vector<mesh::arraybufferparam> params;
			if (!aimh->HasPositions())
			{
				BOOST_LOG_TRIVIAL(error) << "model::load:  no vectex";
				return false;
			}

			int poisitionsize = aimh->mNumVertices;
			if (aimh->HasPositions()) 
			{
				mesh::arraybufferparam param;
				param.data = (float*)aimh->mVertices;
				param.datatype = mesh::VEC3;
				param.offset = 0;
				param.width = 0;
				param.type = mesh::VECTEX;
				params.push_back(param);
				//for (int i = 0; i < poisitionsize; i++)
				//{
				//	printf("%f,%f,%f,\r\n", aimh->mVertices[i][0], aimh->mVertices[i][1], aimh->mVertices[i][2]);
				//}
				//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			}

			if (aimh->HasNormals()) 
			{
				mesh::arraybufferparam param;
				param.data = (float*)aimh->mNormals;
				param.datatype = mesh::VEC3;
				param.offset = 0;
				param.width = 0;
				param.type = mesh::NORMAL;
				params.push_back(param);
			}
			if (aimh->HasTangentsAndBitangents())
			{
				{
					mesh::arraybufferparam param;
					param.data = (float*)aimh->mTangents;
					param.datatype = mesh::VEC3;
					param.offset = 0;
					param.width = 0;
					param.type = mesh::TANGENTS;
					params.push_back(param);
				}
				{
					mesh::arraybufferparam param;
					param.data = (float*)aimh->mBitangents;
					param.datatype = mesh::VEC3;
					param.offset = 0;
					param.width = 0;
					param.type = mesh::BITANGENTS;
					params.push_back(param);
				}
			}

			int colornum = aimh->GetNumColorChannels();
			for (int i = 0; i < colornum; i++)
			{
				mesh::arraybufferparam param;
				param.data = (float*)aimh->mColors[i];
				param.datatype = mesh::VEC4;
				param.offset = 0;
				param.width = 0;
				param.type = mesh::COLOR;
				params.push_back(param);
			}
			 
			int texturenum = aimh->GetNumUVChannels();
			for (int i = 0; i < texturenum; i++)
			{
				mesh::arraybufferparam param;
				param.data = (float*)aimh->mTextureCoords[i];
				switch (aimh->mNumUVComponents[i])
				{
				case 1:
					param.datatype = mesh::VEC1;
					param.offset = 0;
					param.width = sizeof(aiColor3D);
					break;
				case 2:
					param.datatype = mesh::VEC2;
					param.offset = 0;
					param.width = sizeof(aiColor3D);

					//for (int i = 0; i < poisitionsize; i++)
					//{
					//	printf("%f,%f,\r\n", aimh->mTextureCoords[0][i][0], aimh->mTextureCoords[0][i][1]);
					//}
					break;
				case 3:
					param.datatype = mesh::VEC3;
					param.offset = 0;
					param.width = 0;
					break;
				default:
					BOOST_LOG_TRIVIAL(error) << "model::load:  unknow texturecoords type";
					return false;
					break;
				}
				param.type = mesh::TEXTURECOORDS;
				params.push_back(std::move(param));
			}
			if (!mh->initarraybuffer(params, poisitionsize))
				return false;


			std::vector<mesh::elementybufferparam>  elementparams;
			for (int i = 0; i < aimh->mNumFaces; i++)
			{
				mesh::elementybufferparam param;
				param.datatype = Vao::UNSIGNED_INT;
				param.data = aimh->mFaces[i].mIndices;
				param.size = aimh->mFaces[i].mNumIndices;
				elementparams.push_back(std::move(param));
			}
			if ((elementparams.size() > 0) && (!mh->initelementbuffer(elementparams)))
			{
				BOOST_LOG_TRIVIAL(error) << "model::load:  initelementbuffer error";
				return false;
			}

			if (aimh->HasBones())
			{
				//骨骼矩阵列表，数量和骨骼数一致
				std::vector<glm::mat4> matixs;
				//某个骨骼点对应的在mesh中的ID和在bonetables中对应的第几个的ID的对应表，中间量，下一步无用
				std::map<int, int> bonetableslist;
				//某个骨骼对应的全部矩阵和权重数据表
				std::vector<bonetable> bonetables;
				//对应全部的点，如果为-1则不是骨骼点，否则对应骨骼点表bonetables的ID值
				std::vector<int> bonetableID;
				bonetableID.resize(poisitionsize);
				memset(&bonetableID[0], -1, sizeof(int)*bonetableID.size());
				for (int i = 0; i < aimh->mNumBones; i++)
				{
					glm::mat4 mat;
					assimp2glm_mat(aimh->mBones[i]->mOffsetMatrix, mat);
					matixs.push_back(std::move(mat));
					for (int j = 0; j < aimh->mBones[i]->mNumWeights; j++)
					{
						uint vecid = aimh->mBones[i]->mWeights[j].mVertexId;
						if (bonetableslist.find(vecid) == bonetableslist.end()) {
							bonetableslist[vecid] = bonetables.size();
							bonetable table;
							bonetables.push_back(std::move(table));
							bonetableID[vecid] = bonetables.size() - 1;
						}
						int id = bonetableslist[vecid];
						bonetable& bontal = bonetables[id];
						assert(bontal.size < MAX_BONE_SIZE);
						bontal.matixid[bontal.size] = i;
						bontal.matixwieght[bontal.size] = aimh->mBones[i]->mWeights[j].mWeight;
						bontal.size++;
					}
				}
				if (!mh->initbone(matixs, bonetables, bonetableID))
					return false;
			}//aimh->HasBones()


		}//for
		//meshend

		
		//material
		int materialsszie = scene->mNumMaterials;
		m_materials.resize(materialsszie);
		//可以用\或者/表示路径，但是取最远的那一个
		int pathrootpos = max((int)path.find_last_of('\\'), (int)path.find_last_of('/'));
		//包含\或者/的主路径
		std::string rootpath = (pathrootpos != std::string::npos) ? path.substr(0, pathrootpos + 1) : "";
		for (int i = 0; i < materialsszie; i++)
		{
			aiMaterial* aimaterial = scene->mMaterials[i];
			std::map<materialtexture::MATERIAL_DETAIL_TYPE, std::string> files;
			for (int mttype = aiTextureType_DIFFUSE; mttype <= aiTextureType_REFLECTION; mttype++)
			{
				int num = aimaterial->GetTextureCount((aiTextureType)mttype);
				for (int n = 0; n < num; n++)
				{
					aiString file;
					if (AI_SUCCESS != aimaterial->GetTexture((aiTextureType)mttype, 0, &file))
						return false;
					files[(materialtexture::MATERIAL_DETAIL_TYPE)mttype] = rootpath + file.C_Str();
				}
			}
			m_materials[i].reset(new materialtexture);
			if (!((materialtexture*)m_materials[i].get())->init(files))
				return false;
		}//for

		//material end


		//node
		m_root.reset(new node);
		node* node = m_root.get();
		aiNode* ainode = scene->mRootNode;
		pasernode(scene, node, ainode);
		return true;


	}
}