#pragma once

#include <glad/glad.h>

class Debug {
public:
    static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,GLenum severity, GLsizei length,
                                                const GLchar* msg, const void* data);

    static GLenum glCheckError_(const char* file, int line);
};

