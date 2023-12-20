#include "FrameBuffer.h"
#include "Renderer.h"

#define constructor(color, depth)\
    GLCall(glGenFramebuffers(1, &ID));\
    bind();\
    if (cubeTexture)\
        initCube(color, depth);\
    else\
        init2D(color, depth);\
    GLenum status;\
    GLCall(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));\
    if (status != GL_FRAMEBUFFER_COMPLETE)\
        ASSERT(false);\
    unBind()

namespace Graphics {
    FrameBuffer::FrameBuffer(Texture& color, Texture& depth, int _width, int _height, bool cubeTexture)
        :width(_width), height(_height), hasColor(0), hasDepth(0)
    {
        constructor(&color, &depth);
    }
    FrameBuffer::FrameBuffer(Texture& color, bool, int _width, int _height, bool cubeTexture)
        :width(_width), height(_height), hasColor(0), hasDepth(0)
    {
        constructor(&color, 0);
    }
    FrameBuffer::FrameBuffer(bool, Texture& depth, int _width, int _height, bool cubeTexture)
        :width(_width), height(_height), hasColor(0), hasDepth(0)
    {
        constructor(0, &depth);
    }

    FrameBuffer::~FrameBuffer()
    {
        GLCall(glDeleteFramebuffers(1, &ID));
    }

    void FrameBuffer::bind() const
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, ID));
    }

    void FrameBuffer::unBind() const
    {
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void FrameBuffer::init2D(Texture* color, Texture* depth)
    {
        if (color)
        {
            color->height = height;
            color->width = width;
            GLCall(glGenTextures(1, &(color->ID)));
            GLCall(glBindTexture(GL_TEXTURE_2D, color->ID));

            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));

            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color->ID, 0));
            hasColor = true;
        }
        else
        {
#ifdef __EMSCRIPTEN__
            GLCall(glDrawBuffers(1, GL_NONE));
            //GLCall(glReadBuffers(1,GL_NONE));
#else
            GLCall(glDrawBuffer(GL_NONE));
            GLCall(glReadBuffer(GL_NONE));
#endif
        }
        if (depth)
        {
            depth->height = height;
            depth->width = width;
            GLCall(glGenTextures(1, &(depth->ID)));
            GLCall(glBindTexture(GL_TEXTURE_2D, depth->ID));

            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->ID, 0));
            hasDepth = true;
        }
    }

#ifndef __EMSCRIPTEN__
    void FrameBuffer::initCube(Texture* color, Texture* depth)
    {
        if (color)
        {
            color->height = height;
            color->width = width;
            GLCall(glGenTextures(1, &(color->ID)));
            GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, color->ID));

            for (unsigned int i = 0; i < 6; i++)
                GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

            GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color->ID, 0));
            hasColor = true;
        }
        else
        {
            GLCall(glDrawBuffer(GL_NONE));
            GLCall(glReadBuffer(GL_NONE));
        }
        if (depth)
        {
            depth->height = height;
            depth->width = width;
            GLCall(glGenTextures(1, &(depth->ID)));
            GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, depth->ID));

            for (unsigned int i = 0; i < 6; i++)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

            GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->ID, 0));
            hasDepth = true;
        }
    }
#endif
}
#undef constructor