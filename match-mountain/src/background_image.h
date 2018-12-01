#pragma once

#include "asg_base.h"
#include "shader_program.h"
#include "attrib_buffer.h"
#include "mesh.h"
#include "texture.h"
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
    Mesh mesh;
};

}
