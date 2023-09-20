#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#endif
#ifdef __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#endif
#ifdef __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

enum TextColor {
	TEXT_COLOR_BLACK,
	TEXT_COLOR_RED,
	TEXT_COLOR_GREEN,
	TEXT_COLOR_YELLOW,
	TEXT_COLOR_BLUE,
	TEXT_COLOR_MAGENTA,
	TEXT_COLOR_CYAN,
	TEXT_COLOR_WHITE,
	TEXT_COLOR_BRIGHT_BLACK,
	TEXT_COLOR_BRIGHT_RED,
	TEXT_COLOR_BRIGHT_GREEN,
	TEXT_COLOR_BRIGHT_YELLOW,
	TEXT_COLOR_BRIGHT_BLUE,
	TEXT_COLOR_BRIGHT_MAGENTA,
	TEXT_COLOR_BRIGHT_CYAN,
	TEXT_COLOR_BRIGHT_WHITE,
	TEXT_COLOR_COUNT
};

template <typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args... args){
	const char* TextColorTable[TEXT_COLOR_COUNT] = {
		"\x1b[30m", // TEXT_COLOR_BLACK
		"\x1b[31m", // TEXT_COLOR_RED
		"\x1b[32m", // TEXT_COLOR_GREEN
		"\x1b[33m", // TEXT_COLOR_YELLOW
		"\x1b[34m", // TEXT_COLOR_BLUE
		"\x1b[35m", // TEXT_COLOR_MAGENTA 
		"\x1b[36m", // TEXT_COLOR_CYAN
		"\x1b[37m", // TEXT_COLOR_WHITE
		"\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
		"\x1b[91m", // TEXT_COLOR_BRIGHT_RED
		"\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
		"\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
		"\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
		"\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
		"\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
		"\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
	};

	char formatBuffer[8192] = {};
	sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

	char textBuffer[8192] = {};
	sprintf(textBuffer, formatBuffer, args...);

	puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARNING: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);
#define SM_ASSERT(x, msg, ...) {        \
	if(!(x)) {                          \
		SM_ERROR(msg, ##__VA_ARGS__);   \
		DEBUG_BREAK();                  \
		SM_ERROR("Assertion hit!");     \
	}                                   \
};

// Bump Allocator
struct BumpAllocator {
	size_t capacity;
	size_t used;
	char* memory;
};

BumpAllocator make_bump_allocator(size_t size) {
	BumpAllocator ba = {};
	ba.memory = (char*)malloc(size);
	ba.capacity = size;

	if(ba.memory) {
		ba.capacity = size;
		memset(ba.memory, 0, size);
	}
	else {
		SM_ASSERT(false, "Failed to allocate Memory!");
	}

	return ba;
}

char* bumb_alloc(BumpAllocator* bumpAllocator, size_t size) {
	char* result = nullptr;

	size_t allignedSize = (size + 7) & ~ 7;
	if(bumpAllocator->used + allignedSize <= bumpAllocator->capacity) {
		result = bumpAllocator->memory + bumpAllocator->used;
		bumpAllocator->used += allignedSize;
	}
	else {
		SM_ASSERT(false, "BumpAllocator is full");
	}

	return result;
}

// File I/O
long long get_timestamp(char* file) {
	struct stat file_stat = {};
	stat(file, &file_stat);
	return file_stat.st_mtime;
}

bool file_exists(char* filePath) {
	SM_ASSERT(filePath, "No filePath supplied!");

	auto file = fopen(filePath, "rb");
	if(!file) {
		return false;
	}
	fclose(file);

	return true;
}

long get_file_size(char* filePath) {
	SM_ASSERT(filePath, "No filePath supplied!");

	long fileSize = 0;
	auto file = fopen(filePath, "rb");
	if(!file) {
		SM_ERROR("Failed to open file: %s", filePath);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	fclose(file);

	return fileSize;
}

char* read_file(char* filePath, int* fileSize, char* buffer) {
	SM_ASSERT(filePath, "No filePath!");
	SM_ASSERT(fileSize, "No fileSize!");
	SM_ASSERT(buffer, "No buffer!");

	*fileSize = 0;
	auto file = fopen(filePath, "rb");
	if(!file) {
		SM_ERROR("Failed to open file: %s", filePath);
		return nullptr;
	}
	
	fseek(file, 0, SEEK_END);
	*fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	memset(buffer, 0, *fileSize + 1);
	fread(buffer, sizeof(char), *fileSize, file);

	fclose(file);

	return buffer;
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* bumpAllocator) {
	char* file = nullptr;
	long dummy_fileSize = get_file_size(filePath);

	if(dummy_fileSize) {
		char* buffer = bumb_alloc(bumpAllocator, dummy_fileSize + 1);
		file = read_file(filePath, fileSize, buffer);
	}

	return file;
}

void write_file(char* filePath, char* buffer, int size) {
	SM_ASSERT(filePath, "No filePath!");
	SM_ASSERT(buffer, "No buffer!");

	auto file = fopen(filePath, "wb");
	if(!file) {
		SM_ERROR("Failed to open file: %s", filePath);
		return;
	}

	fwrite(buffer, sizeof(char), size, file);
	fclose(file);
}

bool copy_file(char* fileName, char* outputName, char* buffer) {
	int fileSize = 0;
	char* data = read_file(fileName, &fileSize, buffer);

	auto outputFile = fopen(outputName, "wb");
	if(!outputFile) {
		SM_ERROR("Failed to open file: %s", outputName);
		return false;
	}

	int result = fwrite(data, sizeof(char), fileSize, outputFile);
	if(!result) {
		SM_ERROR("Failed to open file: %s", outputName);
		return false;
	}

	fclose(outputFile);

	return true;
}

bool copy_file(char* fileName, char* outputName, BumpAllocator* bumpAllocator) {
	char* file = nullptr;
	long dummy_fileSize = get_file_size(fileName);

	if(dummy_fileSize) {
		char* buffer = bumb_alloc(bumpAllocator, dummy_fileSize + 1);
		return copy_file(fileName, outputName, buffer);
	}

	return false;
}