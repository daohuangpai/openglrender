#pragma once

#include <GLFW/glfw3.h>
#include <windows.h>
#include "windows.h"

namespace GLUI
{
	int getKeyMods(void);
	void ConnectGlf2Window(windows* windows, GLFWwindow* window);

}