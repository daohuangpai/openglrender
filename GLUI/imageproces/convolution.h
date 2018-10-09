#pragma once 
#include "../OGL/util.h"
#include "../OGL/texture.h"
#include "../model/dataarray.h"

namespace GLUI
{
	std::shared_ptr<Texture2D> convolution(IN std::shared_ptr<Texture2D>& src_tex,IN dataarray2D<float>& kernel);
	std::shared_ptr<Texture2D> gray(IN std::shared_ptr<Texture2D>& src_tex);


}