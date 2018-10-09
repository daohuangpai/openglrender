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
struct PointInfo {
		glm::vec3 position;
		glm::vec3 nomral;
		glm::vec2 texture_coordinate;
};
#pragma pack(pop)



void generate_ball_vector(std::vector<std::vector<PointInfo>>& table, float a);
bool MakeArrayBuffer(std::vector<std::vector<PointInfo>>& table, std::shared_ptr<GLUI::buffer>& buffer);

inline bool generate_ArrayBuffer(std::shared_ptr<GLUI::buffer>& buffer, float a) 
{
	std::vector<std::vector<PointInfo>> table;
	generate_ball_vector(table, a);
	return MakeArrayBuffer(table, buffer);
}


}