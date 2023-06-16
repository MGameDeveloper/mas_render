#include <glad/glad.h>
#include "mas/window.h"
#include "mas/log.h"


static void local_window_error_callback(int error, const char* desc)
{
	mas_log::error("[ GLFW ]: %s", desc);
}

void local_on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{

}

mas_window::mas_window(const char* title, const glm::vec2& size, u32 gl_major_version, u32 gl_minor_version)
{
	static bool glfw_initiated = false;
	if (!glfw_initiated)
	{
		if (!glfwInit())
		{
			mas_log::error("[ mas_window::create( %s ) ]: glfw init failed");
			return;
		}

		glfwSetErrorCallback(local_window_error_callback);
		glfw_initiated = true;
	}

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor_version);

	window = glfwCreateWindow(size.x, size.y, title, NULL, NULL);
	if (!window)
	{
		mas_log::error("[ mas_window( %s ) ]: creation failed");
		return;
	}

	this->title = title;

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetKeyCallback(window, local_on_key_callback);

	glViewport(0, 0, size.x, size.y);
}

mas_window::~mas_window()
{
	glfwDestroyWindow(window);
}


void mas_window::set_size(const glm::vec2& size)
{
	glfwSetWindowSize(window, size.x, size.y);
}

void mas_window::set_title(const char* title)
{
	glfwSetWindowTitle(window, title);
}

glm::vec2 mas_window::get_size()
{
	i32 w, h;
	glfwGetWindowSize(window, &w, &h);

	return glm::vec2(w, h);
}

const char* mas_window::get_title()
{
	return title;
}

bool mas_window::is_closing()
{
	return glfwWindowShouldClose(window);
}

void mas_window::swap_buffers()
{
	glfwSwapBuffers(window);
}

void mas_window::poll_events()
{
	glfwPollEvents();
}

void mas_window::activate_as_current_context()
{
	glfwMakeContextCurrent(window);
}

void mas_window::register_input(class mas_input* input)
{
	glfwSetWindowUserPointer(window, input);
}