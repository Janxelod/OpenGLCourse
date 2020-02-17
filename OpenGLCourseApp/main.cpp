#include "pch.h"
#include "string.h"

#include <stdio.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Windows dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, IBO, shader, uniformModel;

bool direction = true;

float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.001f;

bool sizeOfDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertext Shader
static const char* vShader = "								\n\
#version 330												\n\
															\n\
layout (location = 0) in vec3 pos;							\n\
															\n\
out vec4 vCol;												\n\
uniform mat4 model; 										\n\
															\n\
void main()													\n\
{															\n\
	gl_Position = model * vec4(pos, 1.f);					\n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);				\n\
}";

// Fragment Shader
static const char* fShader = "								\n\
#version 330												\n\
															\n\
out vec4 colour;											\n\
															\n\
in vec4 vCol;												\n\
void main()													\n\
{															\n\
	colour = vCol;											\n\
}";

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);
	
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);

		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader) // Make sure always that the shader has been created
	{
		printf("Error Creating shader program! \n");
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);

		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);

		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

void CreateTriangle()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};
	// First create a VAO
	// In OpenGL the center of the screen is the coord 0,0
	GLfloat vertices[] = {
		-1.f, -1.f, 0.f,	// 0
		0.0f, -1.0f, 1.0f,	// 1
		1.f, -1.f, 0.f,		// 2
		0.f, 1.f, 0.f		// 3
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); // This is the Binding of the VertexArray

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // This is the Binding the Vertex Buffer Object
			
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// The first parameters is the same ID for both methods 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // This unbinds the Vertex Buffer Object
	glBindVertexArray(0); // This unbinds the VertexArray
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // This unbinds the Index Buffer Object
}

int main()
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

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

	if (!mainWindow)
	{
		printf("GLFW window creation failed");
		glfwTerminate();
		
		return 1;
	}

	// Get buffer size information
	int bufferWidth, bufferHeight;
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

	CreateTriangle();
	CompileShaders();

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user inputs events
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		curAngle += 0.1f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (sizeOfDirection)
		{
			curSize += 0.001f;
		}
		else
		{
			curSize -= 0.001f;
		}

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeOfDirection != sizeOfDirection;
		}

		// Clear Window
		glClearColor(0.f, 0.5f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		
		glm::mat4 model = glm::mat4(1.0);
		// The order how this operations are applied can change the result
		// 1.- Translate first then rotate second will move the object while rotating.
		// 2.- Rotate first then translate will rotate the translation also. 
		// 3.- Scale first then translate, will scale the position its moving also
		
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
		model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.f, 1.f, 0.f));
		
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));



		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}