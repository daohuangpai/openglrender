#pragma once
#include <windows.h> 
#include "../OGL/lazytool.h"
#include "../common/Signal.h"

namespace GLUI
{


	class timer : public ReceiveObject,public SendObject
	{
	public:
		SET_GET_FUN(LARGE_INTEGER, tick)
		SET_GET_FUN(bool, run)
	private:
		LARGE_INTEGER m_tick;
		bool m_run;
	};


	class timecontroll : public SendObject
	{
	public:

	private:

	};
}

