#pragma once
#include "../OGL/vao.h"
#include "../OGL/FrameBuffer.h"

namespace GLUI
{
	std::shared_ptr<GLUI::Vao> get_image_process_vao();
	std::shared_ptr<GLUI::framebuffer> get_image_process_framebuffer(std::shared_ptr<GLUI::Texture2D>& tex);
	std::shared_ptr<GLUI::Texture2D> makeemptytex2D(std::shared_ptr<GLUI::Texture2D>& tex);

}