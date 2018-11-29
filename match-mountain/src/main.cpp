#include <cstdio>
#include <functional>
#include <cmath>

#include "window.h"
#include "background_image.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
//#include <SDL.h>
//#include <SDL_image.h>
//#include <SDL_ttf.h>
#else
#endif

#include <SDL.h>

#include "asg_gl.h"
using namespace asg;

// Shader sources
static const GLchar* vertexSource = R"(
    attribute vec4 position;
    attribute vec3 color;
    uniform mat4 World;
    varying vec3 vColor;
    void main()
    {
        vColor = color;
      gl_Position = World * vec4(position.xyz, 1.0);
    })";
static const GLchar* fragmentSource = R"(
    #ifdef GL_ES
        precision mediump float;
    #endif
    varying vec3 vColor;
    void main()
    {
      gl_FragColor = vec4 (vColor, 1.0 );
    })";


static std::function<void()> loop;
void main_loop() { loop(); }

int main(int /*argc*/, char */*argv*/[])
{
    Window window;
    asg::BackgroundImage background(ASSETS_DIR"37800 IMG_2844.jpg");
    ShaderProgram shaderProgram(vertexSource, fragmentSource);

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo[2];
    glGenBuffers(2, vbo);

    GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
    GLfloat colors[] = {1.0f, 0.0f, 0.0f
                       , 0.0, 1.0f, 0.0f
                       , 0.0f, 0.0f, 1.0f};

    shaderProgram.bind();

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram.getID(), "position");
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    GLint colorAttrib = glGetAttribLocation(shaderProgram.getID(), "color");
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    float angle = 0;
    bool quit = false;
    loop = [&]
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT){
                quit = true;
                return;
            }
        }

        angle += 3.14159f/512;
        float rot[16] =   {
                           std::cos(angle), 0, std::sin(angle), 0
                         , 0, 1, 0, 0
                         , -std::sin(angle), 0, std::cos(angle), 0
                         , 0, 0, 0, 1};

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        background.render();
        glUseProgram(shaderProgram.getID());checkGL();
        glBindVertexArray(vao);checkGL();
        GLint worldUniform = glGetUniformLocation(shaderProgram.getID(), "World");
        glUniformMatrix4fv(worldUniform, 1, GL_FALSE, rot);
        // Draw a triangle from the 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window.getNativeWindow());
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(!quit)
        main_loop();
#endif

    return 0;
}
