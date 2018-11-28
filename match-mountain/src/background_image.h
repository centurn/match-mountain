#pragma once

#include "asg_base.h"
#include "shader_program.h"
#include "attrib_buffer.h"
#include <memory>

namespace asg{

class Bitmap;
// Full-screen rendering of image background
class BackgroundImage{
public:
    explicit BackgroundImage(const char* src);
    ~BackgroundImage();

    void render();
private:
    void prepareGL();
    void prepareTexture();
    void prepareVAO();
    std::unique_ptr<Bitmap> bitmap;

    uint texture_id = 0;
    uint vao = 0;
    ShaderProgram shaderProgram;
    AttribBuffer positions;
    AttribBuffer texcoords;
};

}
