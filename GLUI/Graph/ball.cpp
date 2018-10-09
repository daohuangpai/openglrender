#include "ball.h"
#include "..\OGL\util.h"
#include <math.h>
#include <boost/log/trivial.hpp>

namespace GLUI
{
	//错误在于最后的点坐标是1.0而立刻被切回下一行的纹理坐标就是0导致错误
	void generate_ball_vector(std::vector<std::vector<PointInfo>>& table, float a)
	{
		int vs = (int)(PI / a);
		int us = (int)(2 * PI / a);
		int totalvs = (vs*a == PI) ? vs : vs + 1;
		int totalus = (us*a == 2 * PI) ? us : us + 1;

		table.resize(totalvs);
		for (int v = 0; v < totalvs; v++)
		{
			table[v].resize(totalus);
			float fv = v*a;
			if (v == totalvs-1) fv = PI;
			for (int u = 0; u < totalus; u++) {
				float fu = u*a;
				if (u == totalus-1) fu = 2 * PI;
				PointInfo& info = table[v][u];
				//这里的PI只是近似，所以sin(2 * PI)或者cos的结果不会准确，只能强制设置值，不然会出现底部缺口
				float sinfv = (fv == PI)?0:sin(fv);
				float cosfu = (fu == 2*PI)?1: cos(fu);
				float sinfu = (fu == 2 * PI)?0:sin(fu);
				float cosfv = (fv == PI) ? 1 : cos(fv);
				//位置
				info.position[0] = sinfv*cosfu;
				info.position[1] = sinfv*sinfu;
				info.position[2] = cosfv;
				//法线1半径大小正好就是坐标了
				info.nomral = info.position;
				//纹理坐标
				info.texture_coordinate[0] = (u == totalus - 1) ?1.0f:(fu / (2 * PI));
				info.texture_coordinate[1] = (v == totalvs - 1) ? 1.0f : fv / PI;
				
			}
		}
	}

	void MakeArrayBufferAccessFun(char* data, int size, void* param)
	{
		std::vector<std::vector<PointInfo>>& table = *(std::vector<std::vector<PointInfo>>*)param;
		PointInfo* ptr = (PointInfo*)data;
		int count = 0;
		bool add = true;
		for (int j = 0; j < table.size() - 1; j++)
		{
			for (int i = 0; i < table[j].size() ; i++)
			{
				int posi = add ? i : table[j].size() - i-1;
				int posj1 = add ? j: j + 1;
				int posj2 = add ? j+1 : j ;
				ptr[count++] = table[posj1][posi];
				ptr[count++] = table[posj2][posi];
				//BOOST_LOG_TRIVIAL(error) << "u:" <<j << "v:" << i << "----" << table[j][i].texture_coordinate[0] << "," << table[j][i].texture_coordinate[1] << "\r\n";
				//BOOST_LOG_TRIVIAL(error) << "u:" << j+1 << "v:" << i << "----" << table[j+1][i].texture_coordinate[0] << "," << table[j + 1][i].texture_coordinate[1] << "\r\n";
			}
			add = !add;
		}
	}

	bool MakeArrayBuffer(std::vector<std::vector<PointInfo>>& table, std::shared_ptr<GLUI::buffer>& buffer)
	{

		if (table.size() < 1) {
			BOOST_LOG_TRIVIAL(error) << "MakeArrayBuffer error:no table";
			return false;
		}

		int pointsize = (table.size() - 1) * (table[0].size()) * 2;
		return buffer->generate(NULL, pointsize * sizeof(PointInfo),
			GLUI::buffer::STATIC_DRAW, GLUI::buffer::ARRAY_BUFFER)
			&& buffer->AccessData(MakeArrayBufferAccessFun, &table);
	}

}