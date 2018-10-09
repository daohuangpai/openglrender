#include "kernels.h"

namespace GLUI
{
	bool generate_gaussian_kernel(IN float Delta, IN unsigned int kernelsize, OUT float* data)
	{
		if (kernelsize % 2 == 0)
		{
			BOOST_LOG_TRIVIAL(error) << "kernelsize should be  odd number";
			return false;
		}

		int centerx = (kernelsize - 1) / 2;
		int centery = centerx;
		float sum = 0;
		for (int j = 0; j < kernelsize; j++)
		{
			for (int i = 0; i < kernelsize; i++)
			{
				float v = exp(-((i - centerx)*(i - centerx) + (j - centery)*(j - centery)) / (2 * Delta*Delta));
				data[j*kernelsize + i] = v;
				sum += v;
			}
		}

		for (int j = 0; j < kernelsize; j++)
		{
			for (int i = 0; i < kernelsize; i++)
			{
				data[j*kernelsize + i] = data[j*kernelsize + i]/ sum;
			}
		}

		//for (int j = 0; j < kernelsize; j++)
		//{
		//	for (int i = 0; i < kernelsize; i++)
		//	{	 
		//		printf("%f,", data[j*kernelsize + i]);
		//	}
		//	printf("\r\n");
		//}
		return true;

	}

	float solbelx_data[] = {
		-1.0,0.0f,1.0f,
		-2.0,0.0f,2.0f,
		-1.0,0.0f,1.0f
	};
	bool generate_solbel_kernel_x(OUT float* data)
	{
		memcpy(data, solbelx_data, sizeof(solbelx_data));
		return true;
	}
	float solbely_data[] = {
		-1.0,-2.0f,-1.0f,
		 0.0,0.0f,0.0f,
		 1.0,2.0f,1.0f
	};
	bool generate_solbel_kernel_y(OUT float* data)
	{
		memcpy(data, solbely_data, sizeof(solbely_data));
		return true;
	}
}