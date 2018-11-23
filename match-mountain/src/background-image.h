#pragma once

#include "mam_base.h"
#include <memory>

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
    void prepareShaders();
    void prepareVAO();
    std::unique_ptr<Bitmap> bitmap;

    uint texture_id = 0;
    uint vao = 0;
    uint shaderProgram  = 0;
};
