#pragma once
#include "view.h"
#include "..\OGL\texture.h"
//#include <GL/gl.h>  
//#include <GL/glu.h>  

namespace GLUI {

class imageview : public view {
public:
	bool setimage(const char* imagename);
	virtual UI_TYPE type() {return IMAGE_VIEW;}
	std::shared_ptr<Texture2D>& tex() { return m_UI_Texture; }
protected:
	virtual void OnDrow(rect* rect) override;
	virtual void OnDrop(int count, const char** paths) override;
protected:
	std::string m_mane;
	std::shared_ptr<Texture2D> m_UI_Texture;
};







}//namespace