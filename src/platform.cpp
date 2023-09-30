#include "platform.h"
#include "yape_lib.h"
#include <dlfcn.h>

void* platform_load_dynamic_library(const char* name) {
	// returns handle to the library if succesfull
	void* result = dlopen(name, RTLD_NOW);
	SM_ASSERT(result, "Failed to load: %s -> Reason: %s", name, dlerror());

	return result;
}

void* platform_load_dynamic_function(void* handle, const char* funcName) {
	void* proc = dlsym(handle, funcName);
	SM_ASSERT(proc, "Failed to load function: %s from %s",handle , funcName);

	return proc;
}

bool platform_free_dynamic_library(void* handle) {
	// dlclose() returns 0 if succesful
	bool freeResult = dlclose(handle);
	SM_ASSERT(!freeResult, "Failed to FreeLibrary -> Reason: %s", dlerror());

	return !freeResult;
}

void platform_set_vsync(bool vsync) {
    glfwSwapInterval(vsync ? 60 : 0 );
}