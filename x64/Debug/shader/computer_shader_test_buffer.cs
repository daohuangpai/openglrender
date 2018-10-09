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

layout(std140,binding = 0) buffer partice
{
	Particecector partices[];
};

layout(std140,binding = 1) buffer Emiter
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
	uint idx = gl_GlobalInvocationID.x;
	partices[idx].position = partices[idx].position + time*partices[idx].speed;
	partices[idx].speed = partices[idx].speed + time*partices[idx].acceleration;
	partices[idx].lifetime = partices[idx].lifetime - time*partices[idx].dec;
	if(partices[idx].lifetime<=0.0)
	{
		partices[idx].position = m_position_range_low + hash31(time + partices[idx].position.x +  partices[idx].position.y +  partices[idx].position.z)*(m_position_range_height-m_position_range_low);
		partices[idx].color = m_color_range_low + hash31(time + partices[idx].color.x +partices[idx].color.y + partices[idx].color.z )*(m_color_range_height-m_color_range_low);
		partices[idx].lifetime = m_lifetime_range_low + hash11(time + partices[idx].lifetime)*(m_lifetime_range_height - m_lifetime_range_low);
		partices[idx].dec = m_dec_range_low + hash11(time + partices[idx].dec)*(m_dec_range_height - m_dec_range_low);
		partices[idx].size = m_size_range_low + hash11(time + partices[idx].size)*(m_size_range_height - m_size_range_low);
		partices[idx].speed = m_speed_range_low + hash31(time + partices[idx].speed.x +partices[idx].speed.y + partices[idx].speed.z )*(m_speed_range_height-m_speed_range_low);
		partices[idx].acceleration = m_acceleration_range_low + hash31(time + partices[idx].acceleration.x +partices[idx].acceleration.y + partices[idx].acceleration.z )*(m_acceleration_range_height-m_acceleration_range_low);	
	}
	
}
