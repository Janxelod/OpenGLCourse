#include "pch.h"
#include "Window.h"


Window::Window()
{
	width = 800;
	height = 600;
}

Window::Window(GLuint windowWidth, GLuint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}

int Window::Initialise()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		printf("Initialization failed");
		glfwTerminate();

		return 1;
	}

	// Set up GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);

	if (!mainWindow)
	{
		printf("GLFW window creation failed");
		glfwTerminate();

		return 1;
	}

	// Get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	// This set the current window to draw // It can be used to draw differents windows if needed
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extensions features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();

		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}