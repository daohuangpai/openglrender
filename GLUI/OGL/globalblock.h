#pragma once
#include "buffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GLUI
{

#pragma pack(push)
#pragma pack(4)
	struct light
	{
		long enable = 0;//�رոù�Դ,����bool��Ϊ������Ҫ���ֽ�
		glm::vec3 m_ambientlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//�����⣬ǰ������ɫ�����һ�������ǻ�����ǿ��
		glm::vec3 m_diffuselightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//�����䣬ǰ������ɫ�����һ�������ǻ�����ǿ��
		glm::vec3 m_specularlightcolor = glm::vec3(1.0f, 1.0f, 1.0f);//����⣬ǰ������ɫ�����һ�������ǻ�����ǿ��
		glm::vec3 m_spot_direction = glm::vec3(0.0f, 0.0f, -1.0f);  //�����Դ�ķ��򣬹�Դ��Ĭ�Ϸ�����(0.0,0.0,-1.0),��ָ��z�Ḻ����
		float m_spot_exponent = 0.0f;//��ʾ�۹�ĳ̶ȣ�Ϊ��ʱ��ʾ���շ�Χ�����������Ĺ���ǿ����ͬ��Ϊ����ʱ��ʾ���������뼯�У����Է��䷽���λ���ܵ�������գ�����λ���ܵ����ٹ��ա���ֵԽ�󣬾۹�Ч����Խ���ԡ�
		float m_spot_cutoff;//��ʾһ���Ƕȣ����ǹ�Դ������������ǽǶȵ�һ�룬��ȡֵ��Χ��0��90֮�䣬Ҳ����ȡ180�������ֵ��ȡֵΪ180ʱ��ʾ��Դ������߸���360�ȣ�����ʹ�þ۹�ƣ���ȫ��Χ���䡣��һ�����Դ��
		glm::vec3 m_position;//���ߵ�λ��
		glm::vec3 m_attenuation = glm::vec3(1.0f, 0.14, 0.07);//˥������ = 1 / (attenuation[0] + attenuation[1] * d + attenuation[2]*attenuation[2] * d)
	};
#pragma pack(pop)

	//�˽ṹ��glsl�е�global��Ӧ
	struct Globalstruct
	{
		glm::mat4 viewProjection;//����ģ������ת������
		glm::mat4 prevViewProjection;//תΪ�ӽǾ���
		glm::mat4 projection;//ͶӰ����
		light lights[8];
	};

	class globalblock {
	public:
		bool Init();
		Globalstruct& block() { return m_block; }
		bool Update();
		bool is_available() { return m_UFBbuffer.get() && m_UFBbuffer->is_available(); }
		static globalblock* Globalblock();
	private:
		std::shared_ptr<buffer> m_UFBbuffer;
		Globalstruct m_block;
		static std::shared_ptr<globalblock> k_globalblock;
	};


}