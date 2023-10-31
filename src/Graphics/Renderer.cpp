#include "Renderer.h"
#include <iostream>

namespace Graphics {
    GLFWwindow* window;
    int width, height;

    void GLClearError()
    {
        while (glGetError() != GL_NO_ERROR);
    }

    void GLAllErrors()
    {
        while (GLenum err = glGetError())
            std::cout << "[OpenGL Error] " << err << std::endl;
    }

    bool GLErrorLog(const char* file, const char* function, int line)
    {
        if (GLenum err = glGetError())
        {
            std::cout << "[OpenGL Error] " << err << "\n"
                << "in file: " << file << "\n"
                << "in function: " << function << "\n"
                << "at line: " << line << std::endl;
            return 0;
        }
        return 1;
    }

    bool init(int _width, int _height, std::string title)
    {
        width = _width;
        height = _height;
        if (!glfwInit())
            return 0;
        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

        if (!window)
        {
            glfwTerminate();
            return 0;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetCursorPos(window, width / 2.0f, height / 2.0f);

        GLFWimage images[1];
        images[0].pixels = stbi_load("res/textures/cat-icon-449x512-h6yiv3wo.png", &images[0].width, &images[0].height, 0, 4);
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            std::cerr << "Glew init Error: \n" << glewGetErrorString(err);
            return 0;
        }
        return 1;
    }
    void cleanUp()
    {
        GLAllErrors();
        glfwSetInputMode(Graphics::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwTerminate();
    }

    Renderer::Renderer(FrameBuffer& _fbo, float fov)
        :camera(glm::vec3(0)), fbo(&_fbo)
    {
        farPlane = 200;
        width = fbo->width;
        height = fbo->height;
        float aspectRatio = (float)(width) / height;
        persp = glm::perspective(glm::radians(fov), aspectRatio, .1f, farPlane);
        ortho = glm::ortho(-aspectRatio * 10, aspectRatio * 10, -10.f, 10.f, -1.f, 50.f);
        if (fbo->hasDepth)
            GLCall(glEnable(GL_DEPTH_TEST));
    }

    Renderer::Renderer(int w, int h, float fov)
        :camera(glm::vec3(0)), width(w), height(h), fbo(0)
    {
        farPlane = 200;
        float aspectRatio = (float)(width) / height;
        persp = glm::perspective(glm::radians(fov), aspectRatio, .1f, farPlane);
        ortho = glm::ortho(-aspectRatio * 10, aspectRatio * 10, -1.f, 1.f, -1.f, 50.f);
        GLCall(glEnable(GL_DEPTH_TEST));
    }

    void Renderer::clear() const
    {
        if (fbo)
        {
            fbo->bind();
            if (fbo->hasColor)
            {
                if (fbo->hasDepth)
                {
                    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
                }
                else
                    GLCall(glClear(GL_COLOR_BUFFER_BIT));
            }
            if (fbo->hasDepth)
                GLCall(glClear(GL_DEPTH_BUFFER_BIT));
        }
        else
        {
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        }
    }

    void Renderer::clear(glm::vec3 clearColor) const
    {
        GLCall(glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0));
        clear();
    }

    void Renderer::draw(const VertexArray& va, const IndexBuffer& ibo, Shader& shader) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        shader.bind();
        va.bind();
        ibo.bind();

        shader.setMatrix("u_Proj", persp);
        shader.setMatrix("u_View", camera.view());
        GLCall(glViewport(0, 0, width, height));
        GLCall(glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, nullptr));
    }

    void Renderer::draw(Object& object) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        if (!object.mesh || !object.isActive)
            return;
        object.bind();

        object.setCameraView(persp, camera.view());
        GLCall(glViewport(0, 0, width, height));
        GLCall(glDrawElements(GL_TRIANGLES, object.getMeshIboCount(), GL_UNSIGNED_INT, nullptr));
    }

    void Renderer::draw(std::vector<Object>& objects) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(0, 0, width, height));
        for (Object& object : objects)
        {
            if (!object.mesh || !object.isActive)
                continue;
            object.bind();

            object.setCameraView(persp, camera.view());
            GLCall(glDrawElements(GL_TRIANGLES, object.getMeshIboCount(), GL_UNSIGNED_INT, nullptr));
        }
    }

    void Renderer::draw(std::unordered_map<std::string, Object>& objects) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(0, 0, width, height));
        for (auto& p : objects)
        {
            auto& object = p.second;
            if (!object.mesh || !object.isActive)
                continue;
            object.bind();

            object.setCameraView(persp, camera.view());
            GLCall(glDrawElements(GL_TRIANGLES, object.getMeshIboCount(), GL_UNSIGNED_INT, nullptr));
        }
    }

    void Renderer::draw2D(const VertexArray& va, const IndexBuffer& ibo, Shader& shader) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        shader.bind();
        va.bind();
        ibo.bind();

        shader.setMatrix("u_Proj", ortho);
        shader.setMatrix("u_View", camera.view());

        GLCall(glViewport(0, 0, width, height));
        GLCall(glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, nullptr));
    }

    void Renderer::draw2D(Object& object) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        if (!object.mesh || !object.isActive)
            return;
        object.bind();

        object.setCameraView(ortho, camera.view());
        GLCall(glViewport(0, 0, width, height));
        GLCall(glDrawElements(GL_TRIANGLES, object.getMeshIboCount(), GL_UNSIGNED_INT, nullptr));
    }

    void Renderer::draw2D(std::vector<Object>& objects) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(0, 0, width, height));
        for (Object& object : objects)
        {
            if (!object.mesh || !object.isActive)
                continue;
            object.bind();

            object.setCameraView(ortho, camera.view());
            GLCall(glDrawElements(GL_TRIANGLES, object.getMeshIboCount(), GL_UNSIGNED_INT, nullptr));
        }
    }

    void Renderer::draw2D(std::unordered_map<std::string, Object>& objects) const
    {
        if (fbo)
            fbo->bind();
        else
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(0, 0, width, height));
        for (auto& p : objects)
        {
            auto& object = p.second;
            if (!object.mesh || !object.isActive)
                continue;
            object.bind();

            object.setCameraView(ortho, camera.view());
            GLCall(glDrawElements(GL_TRIANGLES, object.getMeshIboCount(), GL_UNSIGNED_INT, nullptr));
        }
    }

    /*
    void Renderer::graph(std::vector<glm::vec2> vals, glm::vec2 pos, glm::vec2 dimensions)
    {
        glm::vec2 start = pos - dimensions / 2.f;
        glBegin(GL_LINES);
        for (auto val : vals)
        {
            val = (val + start) / dimensions;
            glVertex2f(val.x, val.y);
        }
        glEnd();
    }
    */
}