#include "assimploader.h"
#include "../OGL/openglerror.h"
#include "../OGL/util.h"
#include "assimpinfo.h"
namespace GLUI
{

	void assimpmesh::clear()
	{
		m_arraybuffer = nullptr;
		m_element = nullptr;
		m_vectex.reset();
		m_normal.reset();
		m_Tangents.reset();
		m_Bitangents.reset();
		m_color.clear();
		m_texturecoords.clear();
		//m_elementIndexs.clear();
	}

	bool assimpmesh::init(aiMesh* mesh)
	{
		clear();
		//first check draw type can be support?
		switch (mesh->mPrimitiveTypes) 
		{
		case aiPrimitiveType_POINT:
			m_drowtype = Vao::POINTS;
			break; 
		case aiPrimitiveType_LINE:
			m_drowtype = Vao::LINES;
			break; 
		case aiPrimitiveType_TRIANGLE:
			m_drowtype = Vao::TRIANGLES;
			break; 
		case aiPrimitiveType_POLYGON:
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init:  cannot support aiPrimitiveType_POLYGON now";
			return false;
			break;
		case _aiPrimitiveType_Force32Bit:
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init: cannot support _aiPrimitiveType_Force32Bit now";
			return false;
			break;
		default:
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init:  unknow type now";
			return false;
			break;
		}


		if (!mesh->HasPositions()) {
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init:  mesh have no position";
			return false;
		}

		//vectex size
		unsigned int vectexsize = sizeof(aiVector3D)*mesh->mNumVertices;
		unsigned int size = vectexsize;

		//normal size
		unsigned int normalsize = mesh->HasNormals()? sizeof(aiVector3D)*mesh->mNumVertices:0;
		size += normalsize;

		//TangentsAndBitangents size
		//这个是使用Bump Mapping来生成法线，计算除了法线方向的其他二维的梯度（纹理图）构建函数 z = f(x,y)
		//具体见https://blog.csdn.net/zhuyingqingfen/article/details/19032939
		//两个都要有，少一个就要自己计算另一个，暂不支持少一个
		unsigned int TangentsAndBitangentssize = mesh->HasTangentsAndBitangents()? 2 * sizeof(aiVector3D)*mesh->mNumVertices:0;
		size += TangentsAndBitangentssize;
		

		unsigned int colornum = mesh->GetNumColorChannels();
		size += colornum * sizeof(aiColor4D)*mesh->mNumVertices;
	 
		//纹理，这边纹理时多少维的由mNumUVComponents数组决定
		unsigned int texcoordsnum = mesh->GetNumUVChannels();
		for (int i = 0; i < texcoordsnum; i++) {
			size += sizeof(ai_real)*mesh->mNumUVComponents[i]*mesh->mNumVertices;
		}
		
			 
		if (mesh->HasBones()) {
			//no deal
		}

		m_AABB.min = glm::vec3(0.0f, 0.0f, 0.0f);
		m_AABB.max = glm::vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			aiVector3D& v = mesh->mVertices[i];
			m_AABB.min = glm::vec3(min(m_AABB.min.x, v.x), min(m_AABB.min.y, v.y), min(m_AABB.min.z, v.z));
			m_AABB.max = glm::vec3(max(m_AABB.max.x, v.x), max(m_AABB.max.y, v.y), max(m_AABB.max.z, v.z));
		}

		m_arraybuffer.reset(new buffer);
		if (!m_arraybuffer->generate(NULL, size, buffer::STATIC_DRAW, buffer::ARRAY_BUFFER))
			return false;

		unsigned int begin = 0;

		std::shared_ptr<buffer> vecbuffer;
		SimpleBufferArray<glm::vec3>::generatebuffer(mesh->mNumVertices, buffer::STATIC_DRAW, buffer::ARRAY_BUFFER, vecbuffer);
	 
		if (!m_vectex.bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
			|| !m_vectex.Update((glm::vec3*)mesh->mVertices, mesh->mNumVertices, 0, false))
			return false;
		begin += m_vectex.bytesize();
		if(  (normalsize>0)
			 &&(!m_normal.bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
			|| !m_normal.Update((glm::vec3*)mesh->mNormals, mesh->mNumVertices, 0, false)))
			return false;
		begin += m_normal.bytesize();

		if (TangentsAndBitangentssize > 0)
		{
			if (!m_Tangents.bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
				|| !m_Tangents.Update((glm::vec3*)mesh->mTangents, mesh->mNumVertices, 0, false))
				return false;
			begin += m_Tangents.bytesize();
			if (!m_Bitangents.bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
				|| !m_Bitangents.Update((glm::vec3*)mesh->mBitangents, mesh->mNumVertices, 0, false))
				return false;
			begin += m_Bitangents.bytesize();
		}
			 
		//color size
		unsigned int colorvectexsize = sizeof(aiColor4D)*mesh->mNumVertices;
		for (int i = 0; i < colornum; i++)
		{
			SimpleBufferArray<glm::vec4> arraybuffer;
			if (!arraybuffer.bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
				|| !arraybuffer.Update((glm::vec4*)mesh->mColors, mesh->mNumVertices, 0, false))
				return false;
			begin += arraybuffer.bytesize();
			m_color.push_back(std::move(arraybuffer));
		}

		//Texture size
		for (int i = 0; i < texcoordsnum; i++)
		{
			std::shared_ptr<BufferArray> arraybuffer;
			switch (mesh->mNumUVComponents[i])
			{
			   case 1:
			   {
				arraybuffer.reset(new SimpleBufferArray<glm::vec1>);
				SimpleBufferArray<glm::vec1>* abf = static_cast<SimpleBufferArray<glm::vec1>*>(arraybuffer.get());
				if (!abf->bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
					|| !abf->Updatewithoffset((char*)mesh->mTextureCoords[i], sizeof(aiVector3D), 0, mesh->mNumVertices, 0))
					return false;
				begin += abf->bytesize();
				m_texturecoords.push_back(std::move(arraybuffer));

			   }
			    break;

			   case 2:
			   {
				   std::shared_ptr<buffer> elebuffer;
				   SimpleBufferArray<glm::vec2>::generatebuffer(mesh->mNumVertices, buffer::STATIC_DRAW, buffer::ARRAY_BUFFER, elebuffer);
				   arraybuffer.reset(new SimpleBufferArray<glm::vec2>);
				   SimpleBufferArray<glm::vec2>* abf = static_cast<SimpleBufferArray<glm::vec2>*>(arraybuffer.get());
				   if (!abf->bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
					   || !abf->Updatewithoffset((char*)mesh->mTextureCoords[i], sizeof(aiVector3D),0,mesh->mNumVertices, 0))
					   return false;
				   begin += abf->bytesize();
				   m_texturecoords.push_back(std::move(arraybuffer));
			   }
			   break;

			   case 3:
			   {
				   arraybuffer.reset(new SimpleBufferArray<glm::vec3>);
				   SimpleBufferArray<glm::vec3>* abf = static_cast<SimpleBufferArray<glm::vec3>*>(arraybuffer.get());
				   if (!abf->bindbuffer(m_arraybuffer, begin, mesh->mNumVertices)
					   || !abf->Update((glm::vec3*)mesh->mTextureCoords[i], mesh->mNumVertices, 0, false))
					   return false;
				   begin += abf->bytesize();
				   m_texturecoords.push_back(std::move(arraybuffer));
			   }
			   break;

			  default:
				  BOOST_LOG_TRIVIAL(error) << "assimpmesh::init:error texturecoord num:"<< mesh->mNumUVComponents[i];
				  return false;
				   break;
			}
			 
		}

		if (size != begin) {
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init: size != begin:" << size<<"!="<< begin;
			return false;
		}

			//计算索引数量
		unsigned int elementsize = 0;
		unsigned int elementbegin = 0;
		if (mesh->HasFaces()) {
				for (int i = 0; i < mesh->mNumFaces; i++) {
					elementsize += sizeof(unsigned int) * mesh->mFaces[i].mNumIndices;
				}
				m_element.reset(new buffer);
				if (!m_element->generate(NULL, elementsize, buffer::STATIC_DRAW, buffer::ELEMENT_ARRAY_BUFFER))
					return false;
				/*for (int i = 0; i < mesh->mNumFaces; i++) {
					SimpleBufferArray<unsigned int> arraybuffer;
					if (!arraybuffer.bindbuffer(m_element, elementbegin, mesh->mFaces[i].mNumIndices)
						|| !arraybuffer.Update(mesh->mFaces[i].mIndices, mesh->mFaces[i].mNumIndices, 0, false))
						return false;
					elementbegin += arraybuffer.bytesize();
					m_elementIndexs.push_back(std::move(arraybuffer));
				}*/
				if (!m_elementIndexs.bindbuffer(m_element, 0, elementsize/sizeof(unsigned int)))
					return false;
				for (int i = 0; i < mesh->mNumFaces; i++) {

					if (!m_elementIndexs.Update(mesh->mFaces[i].mIndices, mesh->mFaces[i].mNumIndices, elementbegin, false))
						return false;
					elementbegin += mesh->mFaces[i].mNumIndices;
					/*{
						if ((mesh->mFaces[i].mIndices[0] >= vectexsize)
							|| (mesh->mFaces[i].mIndices[1] >= vectexsize)
							|| (mesh->mFaces[i].mIndices[2] >= vectexsize))
						{
							return false;
						}
					}*/
				}
		}
		
		m_MaterialIndex = mesh->mMaterialIndex;

		if (!m_vao.BindBuffer( { { m_vectex.buffer(), m_vectex.elementsize(),0,3,GL_FLOAT,GL_FALSE, m_vectex.bufferbegin()} }))
			return false;
		if (!m_vao.BindBuffer( { { m_normal.buffer(), m_normal.elementsize(),1,3,GL_FLOAT,GL_FALSE, m_normal.bufferbegin() } }))
			return false;
		if (m_texturecoords.size() <= 0) {
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init: 必须有纹理坐标:" << size << "!=" << begin;
			return false;
		}
		
		std::shared_ptr<BufferArray> buarray = m_texturecoords[0];
		if (buarray->elementsize() != sizeof(glm::vec2)) {
			BOOST_LOG_TRIVIAL(error) << "assimpmesh::init: 必须是二维纹理:" << size << "!=" << begin;
			return false;
		}

		SimpleBufferArray<glm::vec2>* texbuffer = (SimpleBufferArray<glm::vec2>*)buarray.get();
		if (!m_vao.BindBuffer( { { texbuffer->buffer(), texbuffer->elementsize(), 2,2,GL_FLOAT,GL_FALSE, texbuffer->bufferbegin() } }))
			return false;
	
	
		return true;

	}



	void assimpmesh::drow()
	{

		/*if (!m_vao.BindBuffer({ { m_vectex.buffer(), 0, 0,3,GL_FLOAT,GL_FALSE, m_vectex.bufferbegin() } }))
			return ;
		if (!m_vao.BindBuffer( { { m_arraybuffer,0,1,3,GL_FLOAT,GL_FALSE, m_normal.bufferbegin() } }))
			return ;
		std::shared_ptr<BufferArray> buarray = m_texturecoords[0];
		SampleBufferArray<glm::vec2>* texbuffer = (SampleBufferArray<glm::vec2>*)buarray.get();
		if (!m_vao.BindBuffer({ { texbuffer->buffer(), 0, 2,2,GL_FLOAT,GL_FALSE, texbuffer->bufferbegin() } }))
			return ;*/
 
		//m_vao.DrowArray(m_drowtype, 0, m_vectex.size());
		m_vao.Drawelements(m_element.get(),m_drowtype, m_elementIndexs.size(), elementtype, (const GLvoid*)0);
		//int size = m_elementIndexs.size();
		//for (std::vector<SimpleBufferArray<unsigned int>>::iterator itr = m_elementIndexs.begin();
		//	itr != m_elementIndexs.end(); itr++)
		//{
		//	m_vao.Drawelements(m_drowtype, itr->size(), elementtype, (const GLvoid*)itr->bufferbegin());
		//}
	}
 

	//bool assimpMaterial::init(aiMaterial* material,std::string rootpath)
	//{
	//	for (int i = 0; i < material->mNumProperties; i++)
	//	{
	//		aiMaterialProperty* prop = material->mProperties[i];
	//		//只加载纹理图片
	//		if ((prop->mType == aiPTI_String) 
	//			&& ::strcmp(prop->mKey.data, "$tex.file")
	//			&& (prop->mSemantic>=aiTextureType_NONE)&& (prop->mSemantic <= aiTextureType_UNKNOWN)) {
	//			aiString str;
	//			if (AI_SUCCESS != aiGetMaterialString(material, prop->mKey.data,prop->mSemantic, prop->mIndex, &str)) {
	//				return AI_FAILURE;
	//			}
	//			std::string imagepath = rootpath + str.C_Str();
	//			std::shared_ptr<Texture2D> tex;
	//			tex.reset(Texture2D::CreateTexture2D(imagepath.data()));
	//			if (tex.get()) {
	//				m_Materials[(aiTextureType)prop->mSemantic][prop->mIndex] = tex;
	//			}
	//			else {
	//				BOOST_LOG_TRIVIAL(error) << "assimpMaterial::init: load file failed ,path:" << imagepath.data();
	//				return false;
	//			}
	//		}
	//	}
	//	return true;
	//}

	bool assimpMaterial::init(aiMaterial* material, std::string rootpath)
	{
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString file;
			if (AI_SUCCESS != material->GetTexture(aiTextureType_DIFFUSE, 0, &file))
				return false;
			std::string imagepath = rootpath + file.C_Str();
			std::shared_ptr<Texture2D> tex;
			tex.reset(Texture2D::CreateTexture2D(imagepath.data()));
			if (tex.get()) {
				m_Materials[aiTextureType_DIFFUSE][0] = tex;
			}
			else {
				BOOST_LOG_TRIVIAL(error) << "assimpMaterial::init: load file failed ,path:" << imagepath.data();
				return false;
			}
		}
		return true;
	}


	//image* assimpTexture(aiTexture* texture)
	//{
	//	image* img = new image;
	//	auto imgguide = defaultMakeGuard(img);
	//	//大于0说明是真实数据，不是压缩纹理
	//	if (texture->mHeight > 0)
	//	{
	//		if(texture->CheckFormat(""))
	//	}
	//}

	void assimpnode::init(aiNode* node)
	{
		m_name = node->mName.data;
		m_transformation = glm::make_mat4(&node->mTransformation.a1);
		
		if (node->mNumMeshes > 0) {
			m_Meshes.resize(node->mNumMeshes);
			memcpy(&m_Meshes[0], node->mMeshes, node->mNumMeshes * sizeof(unsigned int));
		}
		 
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			assimpnode* assnode = new assimpnode;
			assnode->init(node->mChildren[i]);
			addchild(assnode);
		}
	}






	bool assimploader::Load(std::string path, int loadtype)
	{

		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, loadtype);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			BOOST_LOG_TRIVIAL(error) << "Assimp failed to load model at path: "<< path;
			return nullptr;
		}

		//m_aiScene = defaultMakeGuard((aiScene*)scene);

		m_meshs.resize(scene->mNumMeshes);
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			if (!m_meshs[i].init(scene->mMeshes[i]))
				return false;
		}

		//可以用\或者/表示路径，但是取最远的那一个
		int pathrootpos = max((int)path.find_last_of('\\'), (int)path.find_last_of('/'));
		//包含\或者/的主路径
		std::string rootpath = (pathrootpos != std::string::npos) ? path.substr(0, pathrootpos + 1) : "";

		m_Material.resize(scene->mNumMaterials);
		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			if (!m_Material[i].init(scene->mMaterials[i], rootpath))
				return false;
		}

		m_noderoot.reset(new assimpnode);
		m_noderoot->init(scene->mRootNode);
		AABB::zero(m_aabb);
		compute_AABB(m_noderoot.get());
		return true;
	}

	void assimploader::drownode(assimpnode* node, std::shared_ptr<shader>& shader, glm::mat4& transformationmat)
	{


		glm::mat4 mat = transformationmat* node->m_transformation;
		for (std::vector<std::unique_ptr<assimpnode>>::iterator itr = node->childs().begin();
			itr != node->childs().end(); itr++)
		{
			drownode(itr->get(), shader, mat);
		}

		shader->set_uniform_mat_4_4("model_matrix", (GLfloat*)&mat[0]);
		for (std::vector<unsigned int>::iterator itr = node->m_Meshes.begin();
			itr != node->m_Meshes.end(); itr++)
		{

			if (!m_Material[m_meshs[*itr].m_MaterialIndex].m_Materials[aiTextureType_DIFFUSE][0].get()) {
					BOOST_LOG_TRIVIAL(error) << "assimploader::drownode::  no texture: ";
			}
			std::shared_ptr<Texture2D>& tex = m_Material[m_meshs[*itr].m_MaterialIndex].m_Materials[aiTextureType_DIFFUSE][0];
			if (tex.get())
				tex->bind();
			
			m_meshs[*itr].drow();
		}
		
	}

	void assimploader::drow(std::shared_ptr<shader>& shader, glm::mat4& transformationmat)
	{
		assimploader::drownode(m_noderoot.get(), shader, transformationmat);
	}

	AABB AABB::combine(AABB& a, AABB& b)
	{
		AABB ret;
		ret.min = glm::vec3(min(a.min.x, b.min.x), min(a.min.y, b.min.y), min(a.min.z, b.min.z));
		ret.max = glm::vec3(max(a.max.x, b.max.x), max(a.max.y, b.max.y), max(a.max.z, b.max.z));
		return ret;
	}

	void AABB::zero(AABB& AABB)
	{
		AABB.min = glm::vec3(0.0f, 0.0f, 0.0f);
		AABB.max = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	
	
	void assimploader::compute_AABB(assimpnode* node)
	{
		for (std::vector<unsigned int>::iterator itr = node->m_Meshes.begin();
			itr != node->m_Meshes.end(); itr++)
		{
			AABB nodeaabb = m_meshs[*itr].get_AABB();
			glm::vec4 min = glm::vec4(nodeaabb.min, 1.0f);
			nodeaabb.min = node->m_transformation * min;
			glm::vec4 max = glm::vec4(nodeaabb.max, 1.0f);
			nodeaabb.max = node->m_transformation * max;
			m_aabb = AABB::combine(nodeaabb, m_aabb);
		}
		
		for (std::vector<std::unique_ptr<assimpnode>>::iterator itr = node->childs().begin();
			itr != node->childs().end(); itr++)
		{
			compute_AABB(itr->get());
		}
	}

}




//void assimpMaterial::clear()
//{
//	m_MaterialProperty.clear();
//}
//
//bool assimpMaterial::init(aiMaterial* material)
//{
//	clear();
//	//其中material->mNumAllocated只是实际分配的空间，类似乘二分配的，这边mNumProperties才是实际有效的数量
//	for (int i = 0; i < material->mNumProperties; i++)
//	{
//		aiMaterialProperty* propery = material->mProperties[i];
//		if (propery->mSemantic<NONE || propery->mSemantic>UNKNOWN) {
//			BOOST_LOG_TRIVIAL(error) << "assimpMaterial::init: unkonw propery->mSemantic,propery->mSemantic should be [NONE,UNKNOWN]";
//			return false;
//		}
//
//		Material_Element met;
//		met.mIndex = propery->mIndex;
//		met.value.SetBuffer((BYTE*)propery->mData, propery->mDataLength);
//		met.type = propery->mType;
//
//		if (propery->mType == aiPTI_String)
//		{
//
//		}
//	}
//}