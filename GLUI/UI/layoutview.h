#pragma once
#include "view.h"
#include "..\OGL\util.h"
namespace GLUI
{
 

	class Hlayout :public view
	{
	public:
		virtual void OnSize() override;
		virtual UI_TYPE type() { return HLAYOUT; }
	};

	class Vlayout :public view
	{
	public:
		virtual void OnSize() override;
		virtual UI_TYPE type() { return VLAYOUT; }
	};

}

