//#ifdef __EMSCRIPTEN__
//#include <emscripten.h>
//#define GL_GLEXT_PROTOTYPES
//#define EGL_EGLEXT_PROTOTYPES
//#else
//#include <GL/glew.h>
//#endif
//#include <GLFW/glfw3.h>
//
//#include <functional>
//#include <vector>
//#include <iostream>
//
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//
//#include "Mesh.h"
//#include "Shader.h"
//#include "Object.h"
//#include "Renderer.h"
//
//std::vector<float> vertices = {
//        -0.6f, -0.4f, 1.0f, 1.f, 0.f, 0.f,
//        0.6f, -0.4f, 1.0f, 0.f, 1.f, 0.f,
//        0.f, 0.6f, 1.0f, 0.f, 0.f, 1.f
//};
//
//std::vector<unsigned int> indices = { 0,1,2 };
//
//
//static void error_callback(int error, const char *description)
//{
//    std::cerr<< "Error: \n" <<description<<std::endl;
//}
//static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GLFW_TRUE);
//}
//
//std::function<void()> loop;
//void main_loop() { loop(); }
//
//void check_error(unsigned int shader)
//{
//    int result;
//    
//    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
//    if (result == GL_FALSE)
//    {
//        GLint log_length;
//        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
//        std::vector<GLchar> log(log_length);
//
//        GLsizei length;
//        glGetShaderInfoLog(shader, log.size(), &length, log.data());
//
//        error_callback(0, log.data());
//    }
//}
//
//static bool init(GLFWwindow* window)
//{
//    if (!window)
//    {
//        glfwTerminate();
//        return 0;
//    }
//    glfwSetKeyCallback(window, key_callback);
//    glfwMakeContextCurrent(window);
//    glfwSwapInterval(1);
//    glfwSetCursorPos(window, 0.0, 0.0);
//
//#ifdef __EMSCRIPTEN__
//    GLenum err = GLEW_OK;
//#else
//    GLenum err = glewInit();
//#endif
//    if (GLEW_OK != err)
//    {
//        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
//        return 0;
//    }
//    return 1;
//}
//
//int main(void)
//{
//    int width = 640,
//        height = 480;
//
//    glfwSetErrorCallback(error_callback);
//    if (!glfwInit())
//        return 1;
//    GLFWwindow* window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
//    if (!init(window))
//        return -1;
//
//    {
//        Mesh tri(vertices, indices, 3, true, false);
//        Shader sh("res/shaders/color.shader");
//        Object o(&tri, &sh, glm::vec3(0));
//        Renderer r(width, height);
//
//        loop = [&] {
//            r.clear();
//            r.draw(o);
//            r.getInput(window);
//            glfwSwapBuffers(window);
//            glfwPollEvents();
//        };
//
//#ifdef __EMSCRIPTEN__
//        emscripten_set_main_loop(main_loop, 0, true);
//#else
//        while (!glfwWindowShouldClose(window))
//            main_loop();
//#endif
//    }
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
//}