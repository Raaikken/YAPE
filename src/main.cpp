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

bool running = true;

typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr;

void update_game(GameState* gamestateIn, RenderData* renderDataIn, Input* inputIn) {
    update_game_ptr(gamestateIn, renderDataIn, inputIn);
}

/* Reloads game file which contains the game logic
 *
 * function: reload_game
 * param: tansientStorage, pointer to memory
 * return: void
*/
void reload_game(BumpAllocator* transientStorage) {
    static void* game;
    static long long lastEditTimestampGame;

    long long currentTimeStampGame = get_timestamp("game");

    if(currentTimeStampGame > lastEditTimestampGame) {
        if(game) {
            bool freeResult = platform_free_dynamic_library(game);
            SM_ASSERT(freeResult, "Failed to free game file");
            game = nullptr;
            SM_TRACE("Freed game file");
        }

        while(!copy_file("game", "game_load", transientStorage)) {
            sleep(10);
        }
        SM_TRACE("Copied game into game_load");

        game = platform_load_dynamic_library("./game_load");
        SM_ASSERT(game, "Failed to load game file");

        update_game_ptr = (update_game_type*)platform_load_dynamic_function(game, "update_game");
        SM_ASSERT(update_game_ptr, "Failed to load update_game function");
        lastEditTimestampGame = currentTimeStampGame;
    }
}

/* process player input
 *
 * function: processInput 
 * param: window, pointer to the window that the input was sent to
 * return: void
*/
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

/* call back function for handling OpenGL debug messages
 *
 * function: gl_debug_callback 
 * param: source, source that produced the message
 *        type, the type of message
 *        id, id of the message
 *        severity, the level of importance
 *        length, length of the message string exluding the null-terminator
 *        message, string containing the message
 *        user, user which triggered the message
 * return: void
*/
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
    if(severity == GL_DEBUG_SEVERITY_LOW ||	severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH) {
        SM_ASSERT(false, "OpenGL Error: %s", message);
    }
    else {
        SM_TRACE(message);
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
        SM_ERROR("Failed to allocate Input");
        return -1;
    }
    gameState = (GameState*)bump_alloc(&persistentStorage, sizeof(GameState));
    if(!gameState) {
        SM_ERROR("Failed to allocate GameState");
        return -1;
    }

    gl_init(&transientStorage);

    while(!glfwWindowShouldClose(glContext.window)) {
        reload_game(&transientStorage);
        glfwGetCursorPos(glContext.window, &input->mousePos.x, &input->mousePos.y);
        glfwPollEvents();

        processInput(glContext.window);
        update_game(gameState, renderData, input);
        gl_render(&transientStorage);

        // Check and call events and swap the buffers
        glfwSwapBuffers(glContext.window);
        clearKeyCodes();

        transientStorage.used = 0;
    }

    gl_terminate();

    return 0;
}