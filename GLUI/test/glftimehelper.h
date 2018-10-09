#pragma once
// GLFW
#include <GLFW/glfw3.h>

class glftimer {
public:
	glftimer()
	{
		oldtime = glfwGetTime();
	}
	double passsec()
	{
		return glfwGetTime() - oldtime;
	}

	void update()
	{
		oldtime = glfwGetTime();
	}
private:
	double oldtime;
};