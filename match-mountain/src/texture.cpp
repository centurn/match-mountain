#include "texture.h"
#include "asg_gl.h"

namespace asg {

Texture::Texture(const char *src)
    : bmp(src)
{

}

void Texture::bind()
{
    prepareGL();
//    log_d("Binding texture: %d", texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);checkGL();
}

void Texture::prepareGL()
{
    if(texture_id != 0)
        return;
    glGenTextures(1, &texture_id);checkGL();
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // texture_data is the source data of your texture, in this case
    // its size is sizeof(unsigned char) * texture_width * texture_height * 4
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.getWidth(), bmp.getHeight()
                 , 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.getData());checkGL();
//    glGenerateMipmap(GL_TEXTURE_2D);
}

}
