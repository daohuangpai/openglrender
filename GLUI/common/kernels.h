#pragma once
#include "../OGL/util.h"
#include "../model/dataarray.h"

namespace GLUI
{
	bool generate_gaussian_kernel(IN float Delta, IN unsigned int kernelsize, OUT float* data);
	inline bool generate_gaussian_kernel(IN float Delta, IN unsigned int kernelsize, OUT dataarray2D<float>& data)
	{
		data.init(kernelsize, kernelsize);
		return generate_gaussian_kernel(Delta, kernelsize, data.get_data());
	}


	bool generate_solbel_kernel_x(OUT float* data);
	inline bool generate_solbel_kernel_x(OUT dataarray2D<float>& data)
	{
		data.init(3, 3);
		return generate_solbel_kernel_x(data.get_data());
	}


	bool generate_solbel_kernel_y(OUT float* data);
	inline bool generate_solbel_kernel_y(OUT dataarray2D<float>& data)
	{
		data.init(3, 3);
		return generate_solbel_kernel_x(data.get_data());
	}


}