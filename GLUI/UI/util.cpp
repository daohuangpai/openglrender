#include "util.h"
#include <algorithm>

namespace GLUI {


	rect rect_operation_and(rect& r1, rect& r2)
	{
		rect result;
		result.x = std::max(r1.x, r2.x);
		result.y = std::max(r1.y, r2.y);
		result.width = std::min(r1.x + r1.width, r2.x + r2.width) - result.x;
		result.height = std::min(r1.y + r1.height, r2.y + r2.height) - result.y;
		return result;
	}

	bool is_overlap(rect& r1, rect& r2)
	{
		rect rect = rect_operation_and(r1, r2);
		return rect_available(rect);
	}


















}