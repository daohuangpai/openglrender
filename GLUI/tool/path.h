#pragma once
#include <windows.h>
#include <string>

namespace PATH
{
	inline char* fontpath() { return "C:/Windows/Fonts/"; }
	std::string modulefilename();
	//返回的路径本身结尾带'\\'或者'/'
	std::string moduledirectory();

	std::string shaderpath();
	std::string imagepath();

}