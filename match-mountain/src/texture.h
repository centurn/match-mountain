#pragma once

#include "asg_render_predef.h"
#include "bitmap.h"

namespace asg{

// The very basic texture. Not much params in API
// since I don't have much plans for texture use
class Texture{
public:
    explicit Texture(const char* src);

    void bind();
    const Bitmap& getBitmap() const{
        return bmp;
    }

private:
    void prepareGL();
    Bitmap bmp;

    Ruint texture_id = 0;
};

}
