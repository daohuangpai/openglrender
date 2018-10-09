#pragma once


#define IN
#define OUT
#define INOUT


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm\gtx\transform.hpp>
#include <memory>
#include <boost/log/trivial.hpp>
#include <string>

#define IN 
#define OUT

#define PI 3.1415926535898
namespace GLUI
{
	template<typename T, typename D>
	inline auto MakeGuard(T* t, D d)
	{
		return std::unique_ptr<T, D>(t, std::move(d));
	}

	template<typename T>
	inline auto defaultMakeGuard(T* t)
	{
		return std::unique_ptr<T>(t);
	}
#define CHECKFALSE(v) if(!v) return false;

#define debug_vec_info(vec)\
       {\
	     std::string debug_vec_info = "vec:";\
         debug_vec_info +=#vec;\
          debug_vec_info +="=(";\
		for (int i = 0; i < vec.length(); i++)\
		{\
			debug_vec_info += std::to_string((vec)[i]);\
			if(i!= vec.length()-1)\
			   debug_vec_info += ",";\
		}\
		debug_vec_info += ")";\
        BOOST_LOG_TRIVIAL(info) << debug_vec_info;\
	}

#define debug_Mat_info(mat)\
{\
	     std::string debug_vec_info = "mat:";\
         debug_vec_info +=#mat;\
          debug_vec_info +="=(";\
		for (int w = 0; w < mat.length(); w++)\
		{\
           for (int h = 0; h < mat[0].length(); h++)\
		{\
			debug_vec_info += std::to_string((mat)[w][h]);\
			if(h!= mat.length()-1)\
			   debug_vec_info += ",";\
		}\
         debug_vec_info += "\r\n";\
        }\
		debug_vec_info += ")";\
        BOOST_LOG_TRIVIAL(info) << debug_vec_info;\
	}

#define debug_common_info(common)\
       {\
	    std::string debug_vec_info = "common:";\
        debug_vec_info +=#common;\
        debug_vec_info +="=";\
        debug_vec_info+=std::to_string(common);\
        BOOST_LOG_TRIVIAL(info) << debug_vec_info;\
	}


#define MAKE_TEXT(text) #text


}

namespace glm {

	//sv绕v旋转angle度
	//详见<glm关于绕向量旋转的向量.nb>
	//只有旋转整个坐标的，这边增加
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> rotatev(vec<3, T, Q> const& sv, T angle, vec<3, T, Q> const& v)
	{
		T const a = angle;
		T const c = cos(a);
		T const s = sin(a);

		vec<3, T, Q> axis(normalize(v));
		vec<3, T, Q> temp((T(1) - c) * axis);

		mat<3, 3, T, Q> Rotate;
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		vec<3, T, Q> Result = Rotate * sv;

		return Result;
	}

}