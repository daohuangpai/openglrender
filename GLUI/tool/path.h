#pragma once
#include <windows.h>
#include <string>

namespace PATH
{
	inline char* fontpath() { return "C:/Windows/Fonts/"; }
	std::string modulefilename();
	//���ص�·�������β��'\\'����'/'
	std::string moduledirectory();

	std::string shaderpath();
	std::string imagepath();

}