#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "mas/time.h"

mas_time::mas_time()
{
	start_time = glfwGetTime();
}

void mas_time::calculate_dt()
{
	dt            = glfwGetTime() - start_time;
	elapsed_time += dt;
	start_time    = glfwGetTime();
}