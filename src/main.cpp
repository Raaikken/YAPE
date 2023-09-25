#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "yape_lib.h"

#include "gl_renderer.cpp"
#include "game.h"
#include "platform.cpp"
#include "platform.h"

using namespace std;

bool running = true;

typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr;

void update_game(RenderData* renderData, Input* input) {
	update_game_ptr(renderData, input);
}

void reload_game_so(BumpAllocator* transientStorage) {
	static void* gameSO;
	static long long lastEditTimestampGameSO;

	long long currentTimeStampGameSO = get_timestamp("game.so");

	if(currentTimeStampGameSO > lastEditTimestampGameSO) {
		if(gameSO) {
			bool freeResult = platform_free_dynamic_library(gameSO);
			SM_ASSERT(freeResult, "Failed to free game.so");
			gameSO = nullptr;
			SM_TRACE("Freed game.so");
		}

		while(!copy_file("game.so", "game_load.so", transientStorage)) {
			sleep(10);
		}
		SM_TRACE("Copied game.so into game_load.so");

		gameSO = platform_load_dynamic_library("game_load.so");
		SM_ASSERT(gameSO, "Failed to load game.so");

		update_game_ptr = (update_game_type*)platform_load_dynamic_function(gameSO, "update_game");
		SM_ASSERT(update_game_ptr, "Failed to load update_game function");
		lastEditTimestampGameSO = currentTimeStampGameSO;
	}
}

void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
	if(severity == GL_DEBUG_SEVERITY_LOW ||	severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH) {
		SM_ASSERT(false, "OpenGL Error: %s", message);
	}
	else {
		SM_TRACE((char*)message);
	}
}

int main() {
	BumpAllocator transientStorage = make_bump_allocator(MB(50));
	BumpAllocator persistentStorage = make_bump_allocator(MB(50));

	renderData = (RenderData*)bump_alloc(&persistentStorage, sizeof(RenderData));
	if(!renderData) {
		SM_ERROR("Failed to allocate RenderData");
		return -1;
	}
	input = (Input*)bump_alloc(&persistentStorage, sizeof(Input));
	if(!input) {
		SM_ERROR("Failed to allocate RenderData");
		return -1;
	}

	gl_init(&transientStorage);

	while(!glfwWindowShouldClose(glContext.window)) {
		reload_game_so(&transientStorage);

		processInput(glContext.window);
		update_game(renderData, input);
		gl_render();

		// Check and call events and swap the buffers
		glfwSwapBuffers(glContext.window);
		glfwPollEvents();

		transientStorage.used = 0;
	}

	gl_terminate();

	return 0;
}