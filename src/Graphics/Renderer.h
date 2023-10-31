#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <GL/glew.h>
#endif
#include <string>
#include "Object.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include <glm/glm.hpp>
#include "stb_image/stb_image.h"

namespace Graphics {
#ifdef __EMSCRIPTEN__
#define ASSERT(x) 
#else
#define ASSERT(x) \
			if(!(x)) __debugbreak();
#endif

#define GLCall(x) \
			x;\
			ASSERT(GLErrorLog(__FILE__,#x,__LINE__));

	void GLClearError();
	void GLAllErrors();
	bool GLErrorLog(const char* file, const char* function, int line);

	extern GLFWwindow* window;
	extern int width;
	extern int height;

	bool init(int _width,int _height, std::string title);
	void cleanUp();

	class Renderer
	{
	public:
		glm::mat4 persp,
			ortho;
		FrameBuffer* fbo;

	public:
		Camera camera;
		int width,
			height;
		float farPlane;

		Renderer(FrameBuffer& _fbo, float fov = 45.f);
		Renderer(int w, int h, float fov = 45.f);

		void clear() const;
		void clear(glm::vec3 clearColor) const;
		void draw(const VertexArray& va, const IndexBuffer& ibo, Shader& shader) const;
		void draw(Object& object) const;
		void draw(std::vector<Object>& objects) const;
		void draw(std::unordered_map<std::string, Object>& objects) const;
		void draw2D(const VertexArray& va, const IndexBuffer& ibo, Shader& shader) const;
		void draw2D(Object& object) const;
		void draw2D(std::vector<Object>& objects) const;
		void draw2D(std::unordered_map<std::string, Object>& objects) const;

		//void graph(std::vector<glm::vec2> vals, glm::vec2 pos, glm::vec2 dimensions);

	};
}

//TODO: Add SIngle Value Cache for OpenGL calls such as viewport and bind