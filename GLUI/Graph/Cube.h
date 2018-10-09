#pragma once
#include <vector>
#include "..\OGL\buffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace GLUI
{
#pragma pack(push)
#pragma pack(4)
	struct CubePointInfo {
		glm::vec3 position;
		glm::vec3 nomral;
		glm::vec3 texture;
	};
#pragma pack(pop)


bool MakeCubeArrayBuffer(glm::vec3 shape, std::shared_ptr<GLUI::buffer>& buffer);
bool MakeCubeArrayBufferFront(glm::vec3 shape, std::shared_ptr<GLUI::buffer>& buffer);
}