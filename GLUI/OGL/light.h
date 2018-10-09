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
	����std140���룬��https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
	����https://blog.csdn.net/jxw167/article/details/55224155
	ע���һ���ǣ�����ԭ���Ǻ���������һ�������������һ��int�������һ��vec3,��Ҫ��֤vec3���ʼλ�õľ�����16��������
	����Ҫ��֤vec3�Ĵ�С��16������ҲҪ��֤vec3����ʼƫ����16��������
	struct
	{
	 ƫ��λ�� 0
	 intռ��4
	 int 4
	 ��һ��ƫ��λ��4
	  vec3 16
	}
	20
	ʵ����Ҫ
	struct
	{
	0
	int 4
	ƫ�ƣ�16-4����λ��
	16
	vec3 16
	}
	32
	��Ҳ������struct lightmodelΪʲôҪ��float m_spot_cutoff�������char emptym[12];//����16�ֽڵ�ԭ����Ϊ��������Ҫ�߽�16�����һ��glm::vec3
	�������int int int float ��һ��16 ��������һ��float�����Ի���Ҫ12���ֽڴճ�һ��16���������ܱ�֤m_spot_direction����ʼ������16�ı���
	*/
#pragma pack(push)
#pragma pack(1) 
	struct lightmodel
	{
		int enable = 0;//�رոù�Դ	
		int directionlight = 0;//��ʾ�Ƿ�ʹ��spot��Դ
		int spotlight = 0;//��ʾ�Ƿ�ʹ��spot��Դ	
		float m_spot_exponent = 0.0f;//��ʾ�۹�ĳ̶ȣ�Ϊ��ʱ��ʾ���շ�Χ�����������Ĺ���ǿ����ͬ��Ϊ����ʱ��ʾ���������뼯�У����Է��䷽���λ���ܵ�������գ�����λ���ܵ����ٹ��ա���ֵԽ�󣬾۹�Ч����Խ���ԡ�	
		float m_spot_cutoff = 0.0f;//��ʾһ���Ƕȣ����ǹ�Դ������������ǽǶȵ�һ�룬��ȡֵ��Χ��0��90֮�䣬Ҳ����ȡ180�������ֵ��ȡֵΪ180ʱ��ʾ��Դ������߸���360�ȣ�����ʹ�þ۹�ƣ���ȫ��Χ���䡣��һ�����Դ��
		char emptym[12];//����16�ֽ�
		glm::vec3 m_spot_direction = glm::vec3(0.0f,1.0f,0.0f);  //�����Դ�ķ��򣬹�Դ��Ĭ�Ϸ�����(0.0,0.0,0.0),����ʹ��
		char emptym_spot_direction[4];//����16�ֽ�	
		glm::vec3 m_position = glm::vec3(1.0f, 0.0f, 0.0f);//���ߵ�λ��
		char emptyposition[4];//����16�ֽ�
		glm::vec3 m_attenuation = glm::vec3(1.0f, 0.045f, 0.0075f );//˥������ = 1 / (attenuation[0] + attenuation[1] * d + attenuation[2]*attenuation[2] * d)
		char emptyattenuation[4];//����16�ֽ�
		glm::vec3 m_ambientlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//�����⣬ǰ������ɫ
		char emptyambient[4];//����16�ֽ�
		glm::vec3 m_diffuselightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//�����䣬ǰ������ɫ
		char emptydiffuse[4];//����16�ֽ�
		glm::vec3 m_specularlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//����⣬ǰ������ɫ
		char emptyspecular[4];//����16�ֽ�
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

/*˥������
����	������	һ����	������
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

