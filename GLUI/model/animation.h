#pragma once
//����һ��1����16�ı���
#define MAX_BONE_SIZE 63

namespace MODEL
{
#pragma pack(push)
#pragma pack(1) 
	struct bonetable {
		int size = 0;
		int matixid[MAX_BONE_SIZE];
		float matixwieght[MAX_BONE_SIZE];
		char empty[4];
	};
#pragma pack(pop)




}