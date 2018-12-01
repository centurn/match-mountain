#pragma once

// This header should be a single entry point to GL

#include "asg_log.h"
#include <GLES3/gl3.h>

#ifndef NDEBUG
    #define ENABLE_GL_CHECKS
#endif

#ifdef ENABLE_GL_CHECKS
inline void checkGLFunc(const char* file, int line){
    GLenum err;
    while( (err = glGetError()) != GL_NO_ERROR){
        log_e("GL Error: 0x%X . Location: %s:%d\n", err, file, line);
        std::abort();
    }
}

#define checkGL() checkGLFunc(__FILE__, __LINE__)

#else

#define checkGL()

#endif

