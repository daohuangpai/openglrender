#include "textureview.h"
#include <boost/log/trivial.hpp>
#include "../OGL/name.h"

namespace GLUI 
{

void textureview::OnDrow(rect* rect)
{
	if (!m_texture.get()||!m_texture->is_available()|| !m_shader.get() || !m_shader->is_available()||!m_vao.is_available()) {
		BOOST_LOG_TRIVIAL(error) << "textureview::OnDrow:   cannot OnDrow using unable texture";
		return;
	}

	GLUI::rect rrect = *rect;
	RelativeP2WorldP(rrect.x, rrect.y);
	WorldP2OpenglP(rrect);
	glViewport(rrect.x, rrect.y, rrect.width, rrect.height);

	m_shader->Use();
	m_texture->bind();
	m_vao.DrowArray(Vao::TRIANGLE_STRIP, 0, m_buffersize);
}


////////////////////////////////////imageview/////////////////////////////////////////

//bool imageview::init(std::shared_ptr<Texture2D>& texture)
//{
//	if (!texture->is_available()) {
//		BOOST_LOG_TRIVIAL(error) << "imageview::init:   cannot OnDrow using unable texture";
//		return false;
//	}
//	m_texture = texture;
//	return true;
//}




/*
GLfloat imageviewvertices[] = {
	-1.0f, -1.0f, 0.0f,0.0f,1.0f,
	-1.0f,1.0f, 0.0f,0.0f,0.0f,
	1.0f, -1.0f, 0.0f,1.0f,1.0f,
	1.0f, 1.0f, 0.0f,1.0f,0.0f
};

bool imageview::init(std::shared_ptr<Texture2D>& texture, rect texturerect,bool fullimage)
{
	if (!texture->is_available()) {
		BOOST_LOG_TRIVIAL(error) << "imageview::init:   cannot OnDrow using unable texture";
		return false;
	}

	rect trect(0,0, texture->Width, texture->Height);
	rect rect = rect_operation_and(texturerect, trect);
	if (!rect_available(rect)) {
		BOOST_LOG_TRIVIAL(error) << "imageview::init:   cannot OnDrow using unable rect";
		return false;
	}

	m_texture = texture;
	m_texturerect = texturerect;
	std::shared_ptr<buffer> buf;
	if (!SimpleBufferArray<imageviewvectex>::generatebuffer(4, buffer::STATIC_DRAW, buffer::ARRAY_BUFFER, buf))
	{
		BOOST_LOG_TRIVIAL(error) << "imageview::init:  generate buffer";
		return false;
	}

	m_sba.bindbuffer(buf,0,4);
	if (!m_sba.AccessData(
		[&](imageviewvectex* ptr, int size)->bool
	{
		memcpy(ptr, imageviewvertices, 4 * sizeof(imageviewvectex));
		if (!fullimage) {
			float x = ((float)m_rect.x) / texture->Width;
			float y = ((float)m_rect.x) / texture->Height;
			float w = ((float)m_rect.width) / texture->Width;
			float h = ((float)m_rect.height) / texture->Width;
			ptr[0].textcoord[0] = x;
			ptr[0].textcoord[1] = h;

			ptr[1].textcoord[0] = x;
			ptr[1].textcoord[1] = y;

			ptr[1].textcoord[0] = w;
			ptr[1].textcoord[1] = h;

			ptr[1].textcoord[0] = w;
			ptr[1].textcoord[1] = y;
		}
		
		return true;
	}
	)) {
		BOOST_LOG_TRIVIAL(error) << "timageview::init:   AccessData failed";
		return false;
	}

	if (m_vao.BindBuffer(buf, 5 * sizeof(GL_FLOAT), { { 0,3,GL_FLOAT,GL_FALSE,0 },
	                                                { 1,2,GL_FLOAT,GL_FALSE,3 * sizeof(GL_FLOAT) } }))
	{
		BOOST_LOG_TRIVIAL(error) << "timageview::init:   BindBuffer failed";
		return false;
	}
	m_buffersize = 4;


	if (!m_shader.get()||!rendershadermanger::sharerendershadermanger()->get(std::string(IMAGEVIEW_SHADER), m_shader))
	{
		m_shader = std::make_shared<rendershader>(new rendershader());
		m_shader->create_program_from_file_extension("shader/UI/imageview_vectex_shader.vs",nullptr, "shader/UI/imageview_fragment_shader.fs");
		if (m_shader->is_available()) {
			rendershadermanger::sharerendershadermanger()->set(std::string(IMAGEVIEW_SHADER), m_shader);
		}
	}

	if (!m_shader->is_available()) {
		BOOST_LOG_TRIVIAL(error) << "timageview::init:  shader failed";
		return false;
	}


	return true;
}


void imageview::OnSize()
{

}
*/
}