#include "texture.h"
#include <boost/log/trivial.hpp>
#include "openglerror.h"
#include "util.h"

namespace GLUI {

	MangerTemplateClassCPP(texturemanger)
	MangerTemplateClassCPP(textur2demanger)
	 
	bool Texture::Active(int unit) {
		if (unit >= 0)
			glActiveTexture(GL_TEXTURE0 + unit);
		return opengl_error(StringStream::StringPrintf("Texture::Active:  failed,active num:%d", unit).data());
	}

	void Texture::getInfoFromNowBindTexture(GLenum target,Texture* texture)
	{
		GLint result = -1;
		glGetTexParameterIiv(target, GL_TEXTURE_MIN_FILTER, &result);
		texture->FilterMin = (FilterMode)result;
		glGetTexParameterIiv(target, GL_TEXTURE_MAG_FILTER, &result);
		texture->FilterMax = (FilterMode)result;
		glGetTexParameterIiv(target, GL_TEXTURE_WRAP_S, &result);
		texture->WrapS = (WrapMode)result;
		glGetTexParameterIiv(target, GL_TEXTURE_WRAP_T, &result);
		texture->WrapT = (WrapMode)result;
		glGetTexParameterIiv(target, GL_TEXTURE_WRAP_R, &result);
		texture->WrapR = (WrapMode)result;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &result);
		texture->Width = result;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &result);
		texture->Height = result;
	}

	 int computebitsPerPixel(GLenum Format)
	{
		int bitsPerPixel = 0;
		switch (Format)
		{
		case GL_LUMINANCE:
		case GL_ALPHA:
			bitsPerPixel = 8;
			break;
		case GL_RGB://GL_RGB转GL_BGR_EXT
		case GL_BGR:
			bitsPerPixel = 24;
			break;
		case GL_RGBA:
		case GL_BGRA:
			bitsPerPixel = 32;
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "in image type unsupport by this framework form now,Format:" << Format;
			break;
		}
		return bitsPerPixel;
	}

	 int perdatabyte(GLenum type)
	 {
		 switch (type)
		 {
		 case GL_UNSIGNED_BYTE:
			 return 1;
			 break;
		 case GL_FLOAT:
			 return 4;
			 break;
		 }
		 return 0;
	 }

	void Texture::Alignment(int width)
	{
		unsigned int bytesPerRow = width * computebitsPerPixel(Format)*(perdatabyte(Type)) / 8;
		if (bytesPerRow % 8 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		else if (bytesPerRow % 4 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		else if (bytesPerRow % 2 == 0)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}


	bool Texture::ApplyParamter(GLenum target)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, WrapS);
		if(GL_TEXTURE_1D!= target)
			glTexParameteri(target, GL_TEXTURE_WRAP_T, WrapT);
		if ((GL_TEXTURE_1D != target)&&(GL_TEXTURE_2D!= target))
		    glTexParameteri(target, GL_TEXTURE_WRAP_R, WrapR);

		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, FilterMin);//当所显示的纹理比加载进来的纹理小时，采用GL_LINEAR的方法来处理
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, FilterMax);//当所显示的纹理比加载进来的纹理大时，采用GL_LINEAR的方法来处理
 
		return opengl_error("ApplyParamter");
	}


	int Texture::get_channle()
	{
		int channle = -1;
		switch (Format){
		case GL_LUMINANCE:
		case GL_ALPHA:
			channle = 1;
			break;
		case GL_RGB:
		case GL_BGR:
			channle = 3;
			break;
		case GL_RGBA:
		case GL_BGRA:
			channle = 4;
			break;

		default:
			BOOST_LOG_TRIVIAL(error) << "unsupport type for output file now ,now support gray rgb rgba";
			break;
		}
		return channle;
	}


	bool Texture::setFilterMode(FilterParameter param, FilterMode mode)
	{
		switch (param) {
		case MIN_FILTER:FilterMin = mode; break;
		case MAG_FILTER:FilterMax = mode; break;
		}
		glTexParameteri(Target(), param, mode);
		bool  ret;
		LOG_OPEGNGL_ERROR(BOOST_LOG_TRIVIAL(error), ret, "ApplyParamter error")
		return ret;
	}

	bool Texture::setWrapMode(WrapParameter param, WrapMode mode)
	{
		switch (param)
		{
		case WRAP_S:WrapS = mode; break;
		case WRAP_T:WrapT = mode; break;
		case WRAP_R:WrapR = mode; break;
		}
		glTexParameteri(Target(), param, mode);
		bool  ret;
		LOG_OPEGNGL_ERROR(BOOST_LOG_TRIVIAL(error), ret, "ApplyParamter error")
		return ret;
	}

	//这里GL_UNSIGNED_BYTE是指加载图像的数据格式，因为我们都是使用图片所以确定为GL_UNSIGNED_BYTE
	//fomat则是图像存在这个纹理中的数据格式，可以将GL_UNSIGNED_BYTE转为float存储
	bool Texture::setInternalFormat(GLenum fomat)
	{
		bool result = true;
		switch (fomat)
		{
		case GL_LUMINANCE:
			InternalFormat = GL_LUMINANCE;
			Format = GL_LUMINANCE;
			Type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGB:
			InternalFormat = fomat;
			Format = GL_RGB;//使用opencv这里要用GL_BGR_EXT转，而这里stb不用
			Type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGBA:
			InternalFormat = fomat;
			Format = GL_RGBA;//使用opencv这里要用GL_BGRA_EXT转，而这里stb不用
			Type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGB32F:
			InternalFormat = fomat;
			Format = GL_RGB;//使用opencv这里要用GL_BGR_EXT转，而这里stb不用
			Type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGB16F:
			InternalFormat = fomat;
			Format = GL_RGB;//使用opencv这里要用GL_BGR_EXT转，而这里stb不用
			Type = GL_FLOAT;
			break;
		case GL_RGBA32F:
			InternalFormat = fomat;
			Format = GL_RGBA;//使用opencv这里要用GL_BGR_EXT转，而这里stb不用
			Type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGBA16F:
			InternalFormat = fomat;
			Format = GL_RGBA;//使用opencv这里要用GL_BGR_EXT转，而这里stb不用
			Type = GL_FLOAT;
			break;
		default:
			BOOST_LOG_TRIVIAL(error) << "unsupport type for input file now ,now support gray rgb rgba";
			result = false;
			break;
		}
		return result;
	}




	bool Texture::get_data(GLenum target, GLuint ID, OUT char** outdataptr)
	{
		*outdataptr = nullptr;
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "unavailable texture";
			return false;
		}
		int channle = get_channle();
		if (channle == -1)
			return false;

		//char * data = (char*) malloc(Width*Height*channle);
		char * data = (char*)malloc((Width*channle * 8 + 31) / 32 * 4 * Height);
		auto datafree = GLUI::MakeGuard(data, free);//保证删除;

		glBindTexture(target, ID);

		glGetError();//清除状态
		glGetTexImage(target, 0, Format, Type, data);
		bool result = glGetError();
		if (result != 0) {
			BOOST_LOG_TRIVIAL(error) << "getting texture error！opengl error:" << opengl_errorcode2string(result);
			return false;
		}
		glBindTexture(target, 0);

		datafree.release();//正确的话就不删除空间
		*outdataptr = data;
		return true;
	}




	typedef unsigned long       DWORD;
	typedef unsigned short      WORD;
	typedef long LONG;
#pragma pack(push)
#pragma pack(2)//不指定2对齐，下面tagBITMAPFILEHEADER会变成4个DWORD
	typedef struct tagBITMAPFILEHEADER {
		WORD    bfType;
		DWORD   bfSize;
		WORD    bfReserved1;
		WORD    bfReserved2;
		DWORD   bfOffBits;
	} BITMAPFILEHEADER;
	typedef struct tagBITMAPINFOHEADER {
		DWORD      biSize;
		LONG       biWidth;
		LONG       biHeight;
		WORD       biPlanes;
		WORD       biBitCount;
		DWORD      biCompression;
		DWORD      biSizeImage;
		LONG       biXPelsPerMeter;
		LONG       biYPelsPerMeter;
		DWORD      biClrUsed;
		DWORD      biClrImportant;
	} BITMAPINFOHEADER;
#pragma pack(pop)
#define BI_RGB        0L

	bool Texture::save_bmp(GLenum target, GLuint ID, const char* path)
	{

		char* data = NULL;
		if (!get_data(target, ID, &data))
			return false;

		auto datafree = GLUI::MakeGuard(data, free);//保证删除;

		char buf[5] = { 0 };
		BITMAPFILEHEADER bmpheader;
		BITMAPINFOHEADER bmpinfo;
		FILE *fp;

		if (fopen_s(&fp, path, "wb+") != 0)
		{
			BOOST_LOG_TRIVIAL(error) << "open file failed!\n";
			return false;
		}

		int channle = get_channle();
		if (channle == -1)
			return false;


		bmpheader.bfType = 0x4d42;
		bmpheader.bfReserved1 = 0;
		bmpheader.bfReserved2 = 0;
		bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		bmpheader.bfSize = bmpheader.bfOffBits + Width*Height*channle;

		bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
		bmpinfo.biWidth = Width;
		bmpinfo.biHeight = Height;
		bmpinfo.biPlanes = 1;
		bmpinfo.biBitCount = 8 * channle;//一个像素的占用byte数
		bmpinfo.biCompression = BI_RGB;//不压缩
		bmpinfo.biSizeImage = (Width*bmpinfo.biBitCount + 31) / 32 * 4 * Height;
		bmpinfo.biXPelsPerMeter = 0;
		bmpinfo.biYPelsPerMeter = 0;
		bmpinfo.biClrUsed = 0;
		bmpinfo.biClrImportant = 0;

		fwrite(&bmpheader, sizeof(bmpheader), 1, fp);
		fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);
		fwrite(data, Width*Height*channle, 1, fp);

		fclose(fp);
		return true;
	}



	bool Texture1D::bind_data(char* data)
	{
		genhandleifnecessary();
		bind();
		Alignment(Width);
		glTexImage1D(GL_TEXTURE_1D, 0, InternalFormat, Width, 0, Format, Type, data);
		if (Mipmapping)
			glGenerateMipmap(Target());
		return ApplyParamter(Target());
	}

	Texture2D* Texture2D::CreateTexture2D(GLUI::image*image)
	{
		Texture2D* tex2d = new Texture2D;
		auto tex2dGuard = defaultMakeGuard(tex2d);
		tex2d->Height = image->m_height;
		tex2d->Width = image->m_width;
		switch (image->m_channle)
		{
		case 1:tex2d->setInternalFormat(GL_LUMINANCE); break;
		case 3:tex2d->setInternalFormat(GL_RGB);  break;
		case 4:tex2d->setInternalFormat(GL_RGBA); break;
		default:return nullptr; BOOST_LOG_TRIVIAL(error) << "Texture2D::CreateTexture2D: image->m_channle unsupport!:"<< image->m_channle; break;
		}

		if (!tex2d->bind_data((char*)image->m_data)) {
			return nullptr;
		}
		tex2dGuard.release();
		return tex2d;
	}

	Texture2D* Texture2D::CreateTexture2D(const char* path)
	{
		std::shared_ptr<GLUI::image> image = GLUI::image::GlobalGet(path);
		if (image.get()) {
			return Texture2D::CreateTexture2D(image.get());
		}
		return nullptr;
	}

	bool Texture2D::bind_data(char* data)
	{
		genhandleifnecessary();
		if (!opengl_error("bind_data0"))
			return false;
		bind();
		if (!opengl_error("bind_data1"))
			return false;
		Alignment(Width);
		if (!opengl_error("bind_data2"))
			return false;
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width,Height, 0, Format, Type, data);
		if (!opengl_error("bind_data"))
			return false;
		if (Mipmapping)
			glGenerateMipmap(Target());
		return ApplyParamter(Target());
	}


	bool Texture2D::Copy(int level, char* data, int x, int y, int width, int height)
	{
		bind();
		Alignment(width);
		glTexSubImage2D(Target(), level, x, y, width, height, Format, Type, data);
		if (!opengl_error("Copy"))
			return false;
		return true;
	}


	bool Texture3D::bind_data(char* data)
	{
		genhandleifnecessary();
		bind();
		Alignment(Width);
		glTexImage3D(GL_TEXTURE_3D, 0, InternalFormat, Width, Height,Depth,0, Format, Type, data);
		if (Mipmapping)
			glGenerateMipmap(Target());
		return ApplyParamter(Target());
	}

	bool cubeTexture::bind_data(std::initializer_list<cubeInfoStruct> cubeInfoStructs)
	{
		genhandleifnecessary();
		bind();
		for (auto cubeinfo = cubeInfoStructs.begin(); cubeinfo != cubeInfoStructs.end(); cubeinfo++){
			Width = cubeinfo->width;
			Height =cubeinfo->height;
			if (!setInternalFormat(cubeinfo->InternalFormat)) {
				return false;
			}
			Alignment(Width);
			glTexImage2D(cubeinfo->face, 0, InternalFormat, Width, Height, 0, Format, Type, cubeinfo->data);
			if (!opengl_error("bind_data"))
				return false;
		}
		if (Mipmapping)
			glGenerateMipmap(Target());
		return ApplyParamter(Target());

	}


	bool Texture2dArray::update_data(char* data, GLenum fomat, int width, int height, int index )
	{
		if (index >= m_size) {
			BOOST_LOG_TRIVIAL(error) << "Texture2dArray::update_data: out of texture size,request:" << index << "size:" << m_size;
			return false;
		}

		genhandleifnecessary();
		Width = width;
		Height = height;
		setInternalFormat(fomat);
		if (!opengl_error("bind_data0"))
			return false;
		bind();
		if (!opengl_error("bind_data1"))
			return false;
		Alignment(Width);
		if (!opengl_error("bind_data2"))
			return false;
	    glTexSubImage3D(Target(), 0, 0, 0,index,Width,Height,1, Format, Type, data);
		if (!opengl_error("bind_data"))
			return false;
		if (Mipmapping)
			glGenerateMipmap(Target());
		return true;
	}

	bool Texture2dArray::bind_data(std::vector<std::shared_ptr<image>>& images)
	{
		m_size = images.size();
		ApplyParamter(Target());
		glTexImage3D(Target(), 0, InternalFormat, Width, Height, images.size(), 0, Format, Type, 0);
		if (!opengl_error("Texture2dArray::bind_data: glTexImage3D create size big texture buffer failed"))
			return false;

		for (int i = 0; i < images.size(); i++)
		{
			GLenum fomat = images[i]->OpengType();
			if (-1 == fomat)
				return false;
			if (!update_data((char*)images[i]->m_data, fomat, images[i]->m_width, images[i]->m_height, i))
				return false;
		}
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	//bool simpleTexture::CreateTextureFromFile(const char* path, TextureCreater* config)
	//{
	//	if (!glIsTexture(m_ID)) {
	//		glGenTextures(1,&m_ID);
	//	}
	//	
	//	TextureCreater texcer;
	//	if (config) {
	//		texcer = std::move(*config);
	//	}
	//	texcer.ID = m_ID;
	//	m_Target = texcer.Target;

	//	return texcer.BindTextureFromFile(path);
	//}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//bool cubeTexture::CreateTextureFromFile(char** imagepaths, int count, TextureCreater* config)
	//{
	//	if (!glIsTexture(m_ID)) {
	//		glGenTextures(1, &m_ID);
	//	}
	//	TextureCreater texcreate;
	//	if (config) {
	//		texcreate = std::move(*config);
	//	}
	//	texcreate.ID = m_ID;
	//	texcreate.Target = GL_TEXTURE_CUBE_MAP;
	//	texcreate.bind();

	//	TextureCreater texcer[6];
	//	int size = std::min(6, count);
	//	for (int i = 0; i < size; i++)
	//	{
	//		if (!imagepaths[i])
	//			continue;
	//		texcer[i].ID = m_ID;
	//		texcer[i].Target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
	//		if (!texcer[i].BindTextureFromFile(imagepaths[i])) 
	//			return false;
	//	}
	//	return texcreate.BindTextureFromData(nullptr);
	//}










}

