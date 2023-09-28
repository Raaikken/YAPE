#pragma once

/* loads a library
 * 
 * function: platform_load_dynamic_library
 * param: name, name of the file loaded
 * return: void*
*/
void* platform_load_dynamic_library(void* name);

/* gets a function from a library
 * 
 * function: platform_load_dynamic_function
 * param: handle, pointer to library
 *        funcName, name of function
 * return: void*
*/
void* platform_load_dynamic_function(void* handle, char* funcName);

/* unloads the library
 * 
 * function: platform_free_dynamic_library
 * param: handle, pointer to library
 * return: bool
*/
bool platform_free_dynamic_library(void* handle);