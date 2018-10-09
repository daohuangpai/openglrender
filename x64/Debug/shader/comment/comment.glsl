#ifndef COMMENT_H
#define COMMENT_H

struct light
{
	bool enable;//关闭该光源
	glm::vec3 m_ambientlightcolor;//环境光，前三个颜色，最后一个数据是环境光强度
	glm::vec3 m_diffuselightcolor;//漫反射，前三个颜色，最后一个数据是环境光强度
	glm::vec3 m_specularlightcolor;//反射光，前三个颜色，最后一个数据是环境光强度
	glm::vec3 m_spot_direction;  //方向光源的方向，光源的默认方向是(0.0,0.0,-1.0),即指向z轴负方向
	float m_spot_exponent;//表示聚光的程度，为零时表示光照范围内向各方向发射的光线强度相同，为正数时表示光照向中央集中，正对发射方向的位置受到更多光照，其它位置受到较少光照。数值越大，聚光效果就越明显。
	float m_spot_cutoff;//表示一个角度，它是光源发射光线所覆盖角度的一半，其取值范围在0到90之间，也可以取180这个特殊值。取值为180时表示光源发射光线覆盖360度，即不使用聚光灯，向全周围发射。即一个点光源。
	glm::vec3 m_position;//光线的位置
	glm::vec3 m_attenuation;//衰减因子 = 1 / (attenuation[0] + attenuation[1] * d + attenuation[2]*attenuation[2] * d)
};

layout (std140, binding = 0) uniform Global
{
  uniform mat4 viewProjection;//物体模型坐标转化矩阵
  uniform mat4 prevViewProjection;//转为视角矩阵
  uniform mat4 projection;//投影矩阵
  uniform light lights[8];
}



#endif