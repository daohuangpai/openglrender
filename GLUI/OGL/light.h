#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "object.h"
#include "globalblock.h"
#include "commonbuffer.h"
#include "shader.h"
namespace GLUI {
	/*
	关于std140对齐，见https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
	或者https://blog.csdn.net/jxw167/article/details/55224155
	注意的一点是，对齐原则是和上面描述一样，但是如果是一个int型下面接一个vec3,就要保证vec3离初始位置的距离是16的整数倍
	不仅要保证vec3的大小是16，而且也要保证vec3的起始偏移是16的整数倍
	struct
	{
	 偏移位置 0
	 int占用4
	 int 4
	 下一个偏移位置4
	  vec3 16
	}
	20
	实际需要
	struct
	{
	0
	int 4
	偏移（16-4）的位置
	16
	vec3 16
	}
	32
	这也是下面struct lightmodel为什么要在float m_spot_cutoff下面加上char emptym[12];//补齐16字节的原因，因为其遇到需要边界16对齐的一个glm::vec3
	而上面的int int int float 是一个16 下面又有一个float，所以还需要12个字节凑成一个16，这样才能保证m_spot_direction的起始坐标是16的倍数
	*/
#pragma pack(push)
#pragma pack(1) 
	struct lightmodel
	{
		int enable = 0;//关闭该光源	
		int directionlight = 0;//表示是否使用spot光源
		int spotlight = 0;//表示是否使用spot光源	
		float m_spot_exponent = 0.0f;//表示聚光的程度，为零时表示光照范围内向各方向发射的光线强度相同，为正数时表示光照向中央集中，正对发射方向的位置受到更多光照，其它位置受到较少光照。数值越大，聚光效果就越明显。	
		float m_spot_cutoff = 0.0f;//表示一个角度，它是光源发射光线所覆盖角度的一半，其取值范围在0到90之间，也可以取180这个特殊值。取值为180时表示光源发射光线覆盖360度，即不使用聚光灯，向全周围发射。即一个点光源。
		char emptym[12];//补齐16字节
		glm::vec3 m_spot_direction = glm::vec3(0.0f,1.0f,0.0f);  //方向光源的方向，光源的默认方向是(0.0,0.0,0.0),即不使用
		char emptym_spot_direction[4];//补齐16字节	
		glm::vec3 m_position = glm::vec3(1.0f, 0.0f, 0.0f);//光线的位置
		char emptyposition[4];//补齐16字节
		glm::vec3 m_attenuation = glm::vec3(1.0f, 0.045f, 0.0075f );//衰减因子 = 1 / (attenuation[0] + attenuation[1] * d + attenuation[2]*attenuation[2] * d)
		char emptyattenuation[4];//补齐16字节
		glm::vec3 m_ambientlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//环境光，前三个颜色
		char emptyambient[4];//补齐16字节
		glm::vec3 m_diffuselightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//漫反射，前三个颜色
		char emptydiffuse[4];//补齐16字节
		glm::vec3 m_specularlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//反射光，前三个颜色
		char emptyspecular[4];//补齐16字节
	};
#pragma pack(pop)

	class lightbuffer
	{
	public:
		bool init(lightmodel* models,int size);
		bool init(int size);
		bool update(lightmodel& model, int index);
		bool insert(lightmodel* model, unsigned int size);
		bool Bind(shader* shader);
		inline bool is_available() { return (m_lightmodelsbuffer.buffer().get()!=nullptr); }
	private:
		SimpleBufferArray<lightmodel> m_lightmodelsbuffer;
	};




}

/*衰减因子
距离	常数项	一次项	二次项
7	1.0	0.7	1.8
13	1.0	0.35	0.44
20	1.0	0.22	0.20
32	1.0	0.14	0.07
50	1.0	0.09	0.032
65	1.0	0.07	0.017
100	1.0	0.045	0.0075
160	1.0	0.027	0.0028
200	1.0	0.022	0.0019
325	1.0	0.014	0.0007
600	1.0	0.007	0.0002
3250	1.0	0.0014	0.000007
*/

