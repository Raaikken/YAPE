#pragma once

bool platform_create_window(int width, int height, char* title);
void platform_update_game();
void* platform_load_gl_functions(char* funcName);
void platform_swap_buffers();
void* platform_load_dynamic_library(void* dll);
void* platform_load_dynamic_function(void* dll, char* funcName);
bool platform_free_dynamic_library(void* dll);