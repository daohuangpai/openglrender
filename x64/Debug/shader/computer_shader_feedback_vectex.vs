#version 440 core
#include "common/random.glsl"
layout (local_size_x = 400) in;

	struct Particecector
	{
	vec3 position;//粒子当前位置
	vec3 color;//粒子颜色
	float size;//粒子大小
	float lifetime;//生存时间
	float dec;//粒子消失的速度
	vec3 speed;//速度，位移方向			
	vec3 acceleration;//加速度;
	};

layout (location = 0) in vec3  partice1_position;
layout (location = 1) in vec3  partice1_color;
layout (location = 2) in vec3  partice1_other;
layout (location = 3) in vec3  partice1_speed;
layout (location = 4) in vec3  partice1_acceleration;

out vec3  partice2_position;
out vec3  partice2_color;
out vec3  partice2_other;
out vec3  partice2_speed;
out vec3  partice2_acceleration;

layout(std140,binding = 0) buffer Emiter
{
	vec3 m_position_range_low;
	vec3 m_position_range_height;
	vec3 m_color_range_low;//粒子颜色
	vec3 m_color_range_height;//粒子颜色
	float m_lifetime_range_low;
	float m_lifetime_range_height;
	float m_dec_range_low;//粒子消失的速度
	float m_dec_range_height;//粒子消失的速度
	float m_size_range_low;//粒子大小
	float m_size_range_height;//粒子大小
	vec3 m_speed_range_low;//速度，位移方向		
	vec3 m_speed_range_height;//速度，位移方向			
	vec3 m_acceleration_range_low;//加速度;
	vec3 m_acceleration_range_height;//加速度;
	int m_particlessize;
};

uniform float time;

void main(void)
{
     Particecector p1 = Particecector(partice1_position,partice1_color,partice1_other[0],partice1_other[1],partice1_other[2],partice1_speed,partice1_acceleration);
     if(p1.lifetime>0)
	 {
	   partice2_position = p1.position + time*p1.speed;
	   partice2_color = p1.color;
	   partice2_speed = p1.speed + time*p1.acceleration;
	   partice2_other = vec3(p1.size,p1.lifetime - time*p1.dec,p1.dec);
	   partice2_acceleration = p1.acceleration;
	 }else
	 {
	    partice2_position = m_position_range_low + hash31(time + p1.position.x +  p1.position.y +  p1.position.z)*(m_position_range_height-m_position_range_low);
	  	partice2_color= m_color_range_low + hash31(time + p1.color.x +p1.color.y + p1.color.z )*(m_color_range_height-m_color_range_low);
		partice2_other[1] = m_lifetime_range_low + hash11(time + p1.lifetime)*(m_lifetime_range_height - m_lifetime_range_low);
		partice2_other[2]  = m_dec_range_low + hash11(time + p1.dec)*(m_dec_range_height - m_dec_range_low);
		partice2_other[0]  = m_size_range_low + hash11(time + p1.size)*(m_size_range_height - m_size_range_low);
		partice2_speed = m_speed_range_low + hash31(time + p1.speed.x +p1.speed.y + p1.speed.z )*(m_speed_range_height-m_speed_range_low);
		partice2_acceleration = m_acceleration_range_low + hash31(time + p1.acceleration.x +p1.acceleration.y +p1.acceleration.z )*(m_acceleration_range_height-m_acceleration_range_low);	
	 }
	
}
