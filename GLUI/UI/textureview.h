#pragma once
#include "view.h"
#include "..\OGL\texture.h"
#include "..\OGL\vao.h"
#include "..\OGL\shader.h"
#include "..\OGL\commonbuffer.h"
namespace GLUI {

class textureview : public view {
public:
	virtual void OnDrow(rect* rect) override;

protected:
	std::shared_ptr<Texture2D> m_texture;
	Vao m_vao;
	std::shared_ptr<rendershader> m_shader;
	int m_buffersize;
};


//class imageview : public textureview {
//public:
//	bool init(std::shared_ptr<Texture2D>& texture);
//};




/*
class imageview : public textureview {
public:
	struct imageviewvectex
	{
		glm::vec3 position;
		glm::vec2 textcoord;
    };
	bool init(std::shared_ptr<Texture2D>& texture,rect rect,bool fullimage);
	virtual void OnSize() override;

private:
	rect m_rect;
	SimpleBufferArray<imageviewvectex> m_sba;
};

*/

}