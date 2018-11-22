#include "mam_log.h"
#include <GLES3/gl3.h>

//#define GL_CHECK(Operation){\
//    Operation;\
//    if(auto err = glGetError() != GL_NO_ERROR){\

//    }\
//}

inline void checkGLFunc(const char* file, int line){
    GLenum err;
    while( (err = glGetError()) != GL_NO_ERROR){
        log_e("GL Error: 0x%X . Location: %s:%d\n", err, file, line);
        //std::abort();
    }
}

#define checkGL() checkGLFunc(__FILE__, __LINE__)
