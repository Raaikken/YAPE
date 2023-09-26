#include "platform.h"
#include "yape_lib.h"
#include <dlfcn.h>

void* platform_load_dynamic_library(char* so) {
	char* path = (char*)"./game_load.so";
	void* result = dlopen(path, RTLD_NOW);
	SM_ASSERT(result, "Failed to load so: %s -> Reason: %s", path, dlerror());

	return result;
}

void* platform_load_dynamic_function(void* so, char* funcName) {
	void* proc = dlsym(so, funcName);
	SM_ASSERT(proc, "Failed to load function: %s from SO", funcName);

	return proc;
}

bool platform_free_dynamic_library(void* so) {
	bool freeResult = dlclose(so);
	SM_ASSERT(!freeResult, "Failed to FreeLibrary -> Reason: %s", dlerror());

	return !freeResult;
}