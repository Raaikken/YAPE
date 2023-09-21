#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "yape_lib.h"

using namespace std;

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user);

void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	printf("Hello, Engine!\n");

	// Initialize & create the window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Yet Another Platformer Engine", NULL, NULL);
	if(window == NULL) {
		printf("Failed to create GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD.\n");
		return -1;
	}

	glViewport(0, 0, 800, 600);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glDebugMessageCallback(&gl_debug_callback, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_DEBUG_OUTPUT);

	BumpAllocator transientStorage = make_bump_allocator(MB(50));

	GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	int fileSize = 0;
	char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, &transientStorage);
	char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, &transientStorage);

	if(!vertShader || !fragShader) {
		SM_ASSERT(false, "Failed to load shaders!");
		glfwTerminate();
		return -1;
	}

	glShaderSource(vertShaderID, 1, &vertShader, 0);
	glShaderSource(fragShaderID, 1, &fragShader, 0);

	glCompileShader(vertShaderID);
	glCompileShader(fragShaderID);

	// Test if shaders compiled succesfully
	{
		int success;
		char shaderLog[2048] = {};

		glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);

		if(!success) {
			glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog);
			SM_ASSERT(false, "Failed to compile Vertex Shaders %s", shaderLog);
		}
	}

	{
		int success;
		char shaderLog[2048] = {};

		glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);

		if(!success) {
			glGetShaderInfoLog(fragShaderID, 2048, 0, shaderLog);
			SM_ASSERT(false, "Failed to compile Fragment Shaders %s", shaderLog);
		}
	}
	
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	
	glAttachShader(shaderProgram, vertShaderID);
	glAttachShader(shaderProgram, fragShaderID);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);

	glUseProgram(shaderProgram);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_GREATER);

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Process Inputs
		processInput(window);

		

		// Check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
	if(severity == GL_DEBUG_SEVERITY_LOW ||	severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH) {
		SM_ASSERT(false, "OpenGL Error: %s", message);
	}
	else {
		SM_TRACE((char*)message);
	}
}