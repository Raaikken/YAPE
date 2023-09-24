#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "yape_lib.h"

#include "gl_renderer.cpp"
#include "game.cpp"

using namespace std;

bool running = true;

void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	BumpAllocator transientStorage = make_bump_allocator(MB(50));

	gl_init(&transientStorage);
	while(!glfwWindowShouldClose(glContext.window)) {
		processInput(glContext.window);
		update_game();
		gl_render();

		// Check and call events and swap the buffers
		glfwSwapBuffers(glContext.window);
		glfwPollEvents();
	}

	gl_terminate();

	return 0;
}

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
	if(severity == GL_DEBUG_SEVERITY_LOW ||	severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH) {
		SM_ASSERT(false, "OpenGL Error: %s", message);
	}
	else {
		SM_TRACE((char*)message);
	}
}