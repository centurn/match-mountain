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
    void enableBlending(bool enable){
        alpha_enabled = enable;
    }
    // Resize the vertex buffer so that the image makes
    // maximum fit to the screen while preserving the aspect ratio
    void fitScreen(float screen_aspect);
private:
    Mesh mesh;
    float texture_aspect;
    bool alpha_enabled = true;
    asg::UniformHandler u_apha;
    std::shared_ptr<AttribBuffer> positions;
};

}
