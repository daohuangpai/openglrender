#pragma once
#include "..\OGL\util.h"

namespace GLUI
{
	struct size
	{
		int width = 0;
		int height = 0;
	};

	struct point
	{
		int x = 0;
		int y = 0;
	};

	struct rect
	{
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
	};
	//���θ߿���С��0����Ч
	inline bool rect_available(rect& rect) { return rect.width && rect.height; }
	//rect����㣬�����غϾ�������
	rect rect_operation_and(rect& r1, rect& r2);
	//�ж����������Ƿ��ص�
	bool is_overlap(rect& r1, rect& r2);


	inline void set_bool_int_statue(int type,int n,bool value)
	{
		//assert(n<sizeof(int))
		if (value)
			type = type | ((int)1)<< n;
		else
			type = type & ((int)0) << n;
	}

	inline bool get_bool_int_statue(int type, int n)
	{
		return (type&((int)1 << n))!=0;
	}
}

