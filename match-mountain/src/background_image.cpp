#include "background_image.h"

#include "bitmap.h"
#include "mam_gl.h"

// Shader sources
static const GLchar* vertexSource = R"(
    attribute vec4 position;
    attribute vec2 texcoord;
    varying vec2 vTexCoord;
    void main()
    {
        vTexCoord = texcoord;
        gl_Position = vec4(position.xy, -1., 1.0);
    })";
static const GLchar* fragmentSource = R"(
    #ifdef GL_ES
        precision mediump float;
    #endif
    varying vec2 vTexCoord;
    uniform sampler2D uTexture;
    void main()
    {
        gl_FragColor = texture2D(uTexture, vTexCoord);
//        gl_FragColor = texture2D(uTexture, vec2(0.5, 0.5));
       //gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );
    })";

namespace asg{

static GLfloat vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
static GLfloat tex_coords[] = {0.0f, 1.0f,  1.0f,  1.0,   0.0f, 0.0f, 1.0f, 0.0f};


BackgroundImage::BackgroundImage(const char *src)
    : bitmap(std::make_unique<Bitmap>(src))
    , positions(make_span(vertices))
    , texcoords(make_span(tex_coords))
{
}

BackgroundImage::~BackgroundImage()
{
}

void BackgroundImage::render()
{
    prepareGL();

    glBindVertexArray(vao);
    shaderProgram.bind();

    glActiveTexture(GL_TEXTURE0 + 0);checkGL();
    glBindTexture(GL_TEXTURE_2D, texture_id);checkGL();

    GLint texUniform = glGetUniformLocation(shaderProgram.getID(), "uTexture");checkGL();
    glUniform1i(texUniform, 0);checkGL();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);checkGL();
}

void BackgroundImage::prepareGL()
{
    if(texture_id != 0){
        return;// Already prepared
    }
    if(bitmap == nullptr){
        return;
    }

    shaderProgram.init(vertexSource, fragmentSource);
    prepareTexture();
    prepareVAO();
}

void BackgroundImage::prepareTexture()
{
    glGenTextures(1, &texture_id);checkGL();
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // texture_data is the source data of your texture, in this case
    // its size is sizeof(unsigned char) * texture_width * texture_height * 4
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->getWidth(), bitmap->getHeight()
                 , 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->getData());checkGL();
//    glGenerateMipmap(GL_TEXTURE_2D); // Unavailable in OpenGL 2.1, use gluBuild2DMipmaps() insteads.

  //  glBindTexture(GL_TEXTURE_2D, 0);
}

void BackgroundImage::prepareVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);checkGL();

//    // Create a Vertex Buffer Object and copy the vertex data to it
//    GLuint vbo[2];
//    glGenBuffers(2, vbo);checkGL();


    shaderProgram.bind();
    GLuint posAttrib = GLuint(glGetAttribLocation(shaderProgram.getID(), "position"));checkGL();
    positions.init();
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);checkGL();
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);checkGL();
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    texcoords.init();
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);checkGL();
    GLuint texcoordAttrib = GLuint(glGetAttribLocation(shaderProgram.getID(), "texcoord"));
    glEnableVertexAttribArray(texcoordAttrib);checkGL();
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);checkGL();
}

}// namespace
