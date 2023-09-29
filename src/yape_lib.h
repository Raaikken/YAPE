#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#define EXPORT_FN __declspec(dllexport)
#endif
#ifdef __linux__
#define DEBUG_BREAK() __builtin_trap()
#define EXPORT_FN
#endif
#ifdef __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#define EXPORT_FN 🤷
#endif

#define BIT(x) 1 << (x)
#define KB(x) ((unsigned long long) 1024 * x)
#define MB(x) ((unsigned long long) 1024 * KB(x))
#define GB(x) ((unsigned long long) 1024 * MB(x))

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
void _log(const char* prefix,const  char* msg, TextColor textColor, Args... args){
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

template<typename T, int N>
struct Array {
    static constexpr int maxElements = N;
    int count = 0;
    T elements[N];

    T& operator[](int idx) {
        SM_ASSERT(idx >= 0, "idx negative");
        SM_ASSERT(idx < count, "idx out of bounds");
        return elements[idx];
    }

    int add(T element) {
        SM_ASSERT(count < maxElements, "Array full");
        elements[count] = element;
        return count++;
    }

    void remove_idx_and_swap(int idx) {
        SM_ASSERT(idx >= 0, "idx negative");
        SM_ASSERT(idx < count, "idx out of bounds");
        elements[idx] = elements[--count];
    }

    void clear() {
        count = 0;
    }

    bool is_full() {
        return count == N;
    }
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

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size) {
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
long long get_timestamp(const char* file) {
    struct stat file_stat = {};
    stat(file, &file_stat);
    return file_stat.st_mtime;
}

bool file_exists(const char* filePath) {
    SM_ASSERT(filePath, "No filePath supplied!");

    auto file = fopen(filePath, "rb");
    if(!file) {
        return false;
    }
    fclose(file);

    return true;
}

long get_file_size(const char* filePath) {
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

char* read_file(const char* filePath, int* fileSize, char* buffer) {
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

char* read_file(const char* filePath, int* fileSize, BumpAllocator* bumpAllocator) {
    char* file = nullptr;
    long dummy_fileSize = get_file_size(filePath);

    if(dummy_fileSize) {
        char* buffer = bump_alloc(bumpAllocator, dummy_fileSize + 1);
        file = read_file(filePath, fileSize, buffer);
    }

    return file;
}

void write_file(const char* filePath, char* buffer, int size) {
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

bool copy_file(const char* fileName, const char* outputName, char* buffer) {
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

bool copy_file(const char* fileName, const char* outputName, BumpAllocator* bumpAllocator) {
    // char* file = 0;
    long dummy_fileSize = get_file_size(fileName);

    if(dummy_fileSize) {
        char* buffer = bump_alloc(bumpAllocator, dummy_fileSize + 1);
        return copy_file(fileName, outputName, buffer);
    }

    return false;
}

// #### //
// MATH //
// #### //

struct Vec2 {
    float x, y;

    Vec2(float x, float y)
        : x(x), y(y) {}

    Vec2()
        : x(0), y(0) {}

    // Operator Overloads
    Vec2 Add(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    Vec2 operator+(const Vec2& other) const {
        return Add(other);
    }
    void operator+=(Vec2 other) {
        x += other.x;
        y += other.y;
    }

    Vec2 Subtract(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    Vec2 operator-(const Vec2& other) const {
        return Subtract(other);
    }
    void operator-=(Vec2 other) {
        x -= other.x;
        y -= other.y;
    }

    Vec2 Divide(const float& scalar) const {
        return Vec2(x / scalar, y / scalar);
    }
    Vec2 operator/(const float& scalar) const {
        return Divide(scalar);
    }
    void operator/=(const float& scalar) {
        x /= scalar;
        y /= scalar;
    }
    Vec2 Divide(const Vec2& scalar) const {
        return Vec2(x / scalar.x, y / scalar.y);
    }
    Vec2 operator/(const Vec2& scalar) const {
        return Divide(scalar);
    }
    void operator/=(const Vec2& scalar) {
        x /= scalar.x;
        y /= scalar.y;
    }

    Vec2 Multiply(const float& multiplier) const {
        return Vec2(x / multiplier, y / multiplier);
    }
    Vec2 operator*(const float& multiplier) const {
        return Multiply(multiplier);
    }
    void operator*=(const float& multiplier) {
        x *= multiplier;
        y *= multiplier;
    }
    Vec2 Multiply(const Vec2& multiplier) const {
        return Vec2(x * multiplier.x, y * multiplier.y);
    }
    Vec2 operator*(const Vec2& multiplier) const {
        return Multiply(multiplier);
    }
    void operator*=(const Vec2& multiplier) {
        x /= multiplier.x;
        y /= multiplier.y;
    }
};

struct IVec2 {
    int x;
    int y;

    Vec2 operator-(Vec2 other) {
        return { x - other.x, y - other.y };
    }
};

struct DVec2 {
    double x;
    double y;

    DVec2 operator-(DVec2 other) {
        return { x - other.x, y - other.y };
    }
};

Vec2 IVec2ToVec2(IVec2 v) {
    return Vec2{(float)v.x, (float)v.y};
}

IVec2 DVec2ToIVec2(DVec2 v) {
    return IVec2{ (int)v.x, (int)v.y };
}

struct Vec4 {
    union {
        float values[4];

        struct {
            float x;
            float y;
            float z;
            float w;
        };

        struct {
            float r;
            float g;
            float b;
            float a;
        };
    };

    float& operator[](int idx) {
        return values[idx];
    }
};

class CVec2 {
public:
    float x;
    float y;

    CVec2();

    
private:

};

struct Mat4 {
    union {
        Vec4 values[4];
        struct {
            float ax;
            float bx;
            float cx;
            float dx;

            float ay;
            float by;
            float cy;
            float dy;

            float az;
            float bz;
            float cz;
            float dz;

            float aw;
            float bw;
            float cw;
            float dw;
        };
    };

    Vec4& operator[](int col) {
        return values[col];
    }
};

Mat4 ortographic_projection(float left, float right, float top, float bottom) {
    Mat4 result = {};

    result.aw = -(right + left) / (right - left);
    result.bw = (top + bottom) / (top - bottom);
    result.cw = 0.0f; // Near Plane

    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
    result[2][2] = 1.0f / (1.0f - 0.0f);
    result[3][3] = 1.0f;

    return result;
}