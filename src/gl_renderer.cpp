#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "yape_lib.h"
#include "render_interface.h"
#include "input.h"

const char* TEXTURE_PATH = "assets/textures/TEXTURE_ATLAS.png";

struct GLContext {
	GLFWwindow* window;
	GLuint programID;
	GLuint textureID;
	GLuint transformSBOID;
	GLuint screenSizeID;
	GLuint orthoProjectionID;
};

static GLContext glContext;
int screenWidth, screenHeight;

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

bool gl_init(BumpAllocator* transientStorage) {
	printf("Hello, OpenGL!\n");

	// Initialize & create the window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glContext.window = glfwCreateWindow(1280, 720, "Yet Another Platformer Engine", NULL, NULL);
	if(glContext.window == NULL) {
		printf("Failed to create GLFW window.\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(glContext.window);

	glfwGetWindowSize(glContext.window, &input->screenSize.x, &input->screenSize.y);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD.\n");
		return false;
	}

	glViewport(0, 0, input->screenSize.x, input->screenSize.y);

	glfwSetFramebufferSizeCallback(glContext.window, framebuffer_size_callback);
	glDebugMessageCallback(&gl_debug_callback, nullptr);
    glfwSetKeyCallback(glContext.window, key_callback);
    glfwSetCursorPosCallback(glContext.window, cursor_position_callback);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_DEBUG_OUTPUT);

	GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	int fileSize = 0;
	char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
	char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);

	if(!vertShader || !fragShader) {
		SM_ASSERT(false, "Failed to load shaders!");
		glfwTerminate();
		return false;
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
	
	GLuint textureID;
	glContext.programID = glCreateProgram();
	
	glAttachShader(glContext.programID, vertShaderID);
	glAttachShader(glContext.programID, fragShaderID);
	glLinkProgram(glContext.programID);

	glDetachShader(glContext.programID, vertShaderID);
	glDetachShader(glContext.programID, fragShaderID);
	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load textures with STBI
	{
		int width, height, channels;
		char* data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);

		if(!data) {
			SM_ASSERT(false, "Failed to load textures!");
			return false;
		}

		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	// Transform Storage Buffer
	{
		glGenBuffers(1, &glContext.transformSBOID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformSBOID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, renderData->transforms, GL_DYNAMIC_DRAW);
	}

	// Uniforms
	{
		glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");
		glContext.orthoProjectionID = glGetUniformLocation(glContext.programID, "orthoProjection");
	}

	glEnable(GL_FRAMEBUFFER_SRGB);
	glDisable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);

	glUseProgram(glContext.programID);

	return true;
}

/* render a frame
 * 
 * function: gl_render
 * param: none
 * return: void
*/
void gl_render() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Vec2 screenSize = {(float)screenWidth, (float)screenWidth};
	glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

	OrthographicCamera2D camera = renderData->gameCamera;
	Mat4 orthoProjection = ortographic_projection(camera.position.x - camera.dimensions.x / 2.0f,
												  camera.position.x + camera.dimensions.x / 2.0f,
												  camera.position.y - camera.dimensions.y / 2.0f,
												  camera.position.y + camera.dimensions.y / 2.0f);
	glUniformMatrix4fv(glContext.orthoProjectionID, 1, GL_FALSE, &orthoProjection.ax);

	// Opaque Objects
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData->transformCount, renderData->transforms);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transformCount);	

		renderData->transformCount = 0;
	}
}

/* calls glfwTerminate to safely terminate the window
 * 
 * function: gl_terminate
 * param: none
 * return: void
*/
void gl_terminate() {
	glfwTerminate();
}

/* callback funtion for when window changes its size
 * matches the size of the viewport to the size of the window
 * and stores the new size into a variable
 * 
 * function: framebuffer_size_callback
 * param: window, ptr to the window
 *        width, the new width of the window
 *        height, the new height of the window
 * return: void
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	glfwGetWindowSize(glContext.window, &input->screenSize.x, &input->screenSize.y);
}