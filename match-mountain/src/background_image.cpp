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
BackgroundImage::BackgroundImage(const char *src)
    : bitmap(std::make_unique<Bitmap>(src))
{
}

BackgroundImage::~BackgroundImage()
{
}

void BackgroundImage::render()
{
    prepareGL();

    glBindVertexArray(vao);
    glUseProgram(shaderProgram);checkGL();

    glActiveTexture(GL_TEXTURE0 + 0);checkGL();
    glBindTexture(GL_TEXTURE_2D, texture_id);checkGL();

    GLint texUniform = glGetUniformLocation(shaderProgram, "uTexture");checkGL();
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
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);checkGL();

    prepareShaders();
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

void BackgroundImage::prepareShaders()
{
    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);checkGL();
    glCompileShader(vertexShader);

    GLint vertex_compiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertex_compiled);
    if (vertex_compiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(vertexShader, 1024, &log_length, message);
        log_e("VS compile failed error: %s\n", message);
    }else{
        log_d("VS Compile OK\n");
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    GLint fragment_compiled;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragment_compiled);
    if (fragment_compiled != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(fragmentShader, 1024, &log_length, message);
        log_e("FS compile failed error: %s\n", message);
    }else{
        log_d("VS Compile OK\n");
    }

    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();checkGL();
    glAttachShader(shaderProgram, vertexShader);checkGL();
    glAttachShader(shaderProgram, fragmentShader);checkGL();
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);checkGL();

    GLint linkResult;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkResult);

    if (linkResult == GL_FALSE){
        log_e("Failed to link shader program\n");
    }

    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &linkResult);
    if (linkResult == GL_FALSE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(shaderProgram, 1024, &log_length, message);
        log_e("Error validating shader: %s\n", message);
    }
}

void BackgroundImage::prepareVAO()
{

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo[2];
    glGenBuffers(2, vbo);checkGL();

    GLfloat vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat texcoords[] = {0.0f, 1.0f,  1.0f,  1.0,   0.0f, 0.0f, 1.0f, 0.0f};

    glUseProgram(shaderProgram);checkGL();
    GLuint posAttrib = GLuint(glGetAttribLocation(shaderProgram, "position"));checkGL();
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);checkGL();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);checkGL();
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);checkGL();
    GLuint texcoordAttrib = GLuint(glGetAttribLocation(shaderProgram, "texcoord"));
    glEnableVertexAttribArray(texcoordAttrib);checkGL();
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);checkGL();
}

